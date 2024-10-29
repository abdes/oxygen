//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "registry.h"
#include "detail/registry_impl.h"

// Singleton instance accessor
auto oxygen::log::Registry::Instance() -> Registry & {
  // Guaranteed to be lazy-initialized and thread-safe
  static Registry instance;
  return instance;
}

inline void oxygen::log::Registry::SetLogLevel(
    spdlog::level::level_enum log_level) const {
  pimpl_->SetLogLevel(log_level);
}

void oxygen::log::Registry::SetLogFormat(const std::string &log_format) const {
  pimpl_->SetLogFormat(log_format);
}

auto oxygen::log::Registry::GetLogger(
    std::string const &name) const -> spdlog::logger & {
  return pimpl_->GetLogger(name);
}

void oxygen::log::Registry::PushSink(spdlog::sink_ptr sink) const {
  pimpl_->PushSink(std::move(sink));
}

void oxygen::log::Registry::PopSink() const {
  pimpl_->PopSink();
}

oxygen::log::Registry::Registry() : pimpl_(new detail::RegistryImpl()) {
}
