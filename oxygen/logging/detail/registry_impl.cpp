//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "registry_impl.h"
#include "../delegating_sink.h"
#include "registered_logger.h"

using oxygen::log::DelegatingSink;

namespace {
auto make_delegating_sink() -> std::shared_ptr<DelegatingSink> {
#if defined _WIN32 && !defined(__cplusplus_winrt)
  const auto default_sink =
      std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
  auto default_sink =
      std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif

  return std::make_shared<DelegatingSink>(default_sink);
}
} // namespace

oxygen::log::detail::RegistryImpl::RegistryImpl()
    : delegating_sink_(make_delegating_sink()) {

  const char *const DEFAULT_LOG_FORMAT =
      "[%H:%M:%S %z] [%-20n] [%^--%L--%$] [thread %t] %v";
  // "[%Y-%m-%d %T.%e] [%t] [%^%l%$] [%n] %v";

  SetLogFormat(DEFAULT_LOG_FORMAT);

  loggers_.emplace("misc", Logger("misc", delegating_sink_));
  loggers_.emplace("testing", Logger("testing", delegating_sink_));
  loggers_.emplace("main", Logger("main", delegating_sink_));
}

void oxygen::log::detail::RegistryImpl::RegistryImpl::PushSink(
    spdlog::sink_ptr sink) {
  std::lock_guard<std::mutex> lock(sinks_mutex_);
  // Push the current sink on the stack and use the new one
  sinks_.emplace(delegating_sink_->SwapSink(std::move(sink)));
}

void oxygen::log::detail::RegistryImpl::PopSink() {
  std::lock_guard<std::mutex> lock(sinks_mutex_);
  auto &sinks = sinks_;
  if (!sinks.empty()) {
    const auto &sink = sinks.top();
    // Assign this previous sink to the delegating sink
    delegating_sink_->SwapSink(sink);
    sinks.pop();
  }
}

void oxygen::log::detail::RegistryImpl::SetLogLevel(
    spdlog::level::level_enum log_level) {
  std::lock_guard<std::recursive_mutex> lock(loggers_mutex_);
  for (auto &log : loggers_) {
    // Thread safe
    log.second.SetLevel(log_level);
  }
}

void oxygen::log::detail::RegistryImpl::SetLogFormat(
    const std::string &log_format) {
  std::lock_guard<std::mutex> lock(sinks_mutex_);
  delegating_sink_->set_pattern(log_format);
}

auto oxygen::log::detail::RegistryImpl::GetLogger(
    std::string const &name) -> spdlog::logger & {
  std::lock_guard<std::recursive_mutex> lock(loggers_mutex_);
  auto search = loggers_.find(name);
  if (search == loggers_.end()) {
    const auto new_logger =
        loggers_.emplace(name, Logger(name, delegating_sink_));
    search = new_logger.first;
  }
  return *(search->second.logger_);
}
