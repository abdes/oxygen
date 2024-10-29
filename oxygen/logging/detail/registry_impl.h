//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <stack>         // for stacking sinks
#include <string>        // for std::string
#include <unordered_map> // for the collection of loggers

#include "spdlog/spdlog.h"

#include "oxygen/base/macros.h"
#include "registered_logger.h"

namespace oxygen::log {

class DelegatingSink;

namespace detail {
class RegistryImpl {
public:
  RegistryImpl();
  ~RegistryImpl() = default;

  OXYGEN_MAKE_NON_COPYABLE(RegistryImpl);
  OXYGEN_DEFAULT_MOVABLE(RegistryImpl);

  /*!
   * @brief Sets the minimum log severity required to print messages.
   *
   * Messages below this loglevel will be suppressed.
   *
   * @param [in] log_level the logging severity to be applied to all registered
   * loggers.
   */
  void SetLogLevel(spdlog::level::level_enum log_level);

  /*!
   * @brief Change the format string used by the registered loggers.
   *
   * @param [in] log_format new format string.
   *
   * @see https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
   */
  void SetLogFormat(const std::string &log_format);

  /*!
   * @brief Get a logger by its name.
   *
   * This method automatically registers a new logger for the given name if no
   * corresponding one has been already registered.
   *
   * @param [in] name the name of the logger to fetch.
   *
   * @return The logger corresponding to the given name.
   */
  auto GetLogger(std::string const &name) -> spdlog::logger &;

  /*!
   * @brief Use the given sink for all subsequent logging operations until a
   * call to PopSink() is made.
   *
   * This method, together with PopSink(), enable to switch sinks at runtime
   * temporarily or permanently for all registered loggers.
   *
   * @param [in] sink new sink to replace the existing one. The existing one is
   * pushed on top of an internally managed stack of sinks.
   *
   * @see PopSink()
   */
  void PushSink(spdlog::sink_ptr sink);

  /*!
   * @brief Restore the top of sinks stack as the current sink for all
   * registered loggers.
   *
   * @see PushSink(spdlog::sink_ptr)
   */
  void PopSink();

private:
  /// The collection of registered loggers.
  std::unordered_map<std::string, Logger> loggers_;
  /// Synchronize access to the collection of loggers.
  std::recursive_mutex loggers_mutex_;

  /// API access to the stack of sinks. We don't do any expensive initialization
  /// here, so no need for a second level of access.
  std::stack<spdlog::sink_ptr> sinks_;
  /// A synchronization object for concurrent access to the collection of sinks.
  std::mutex sinks_mutex_;

  /// API access to the delegating sink.
  std::shared_ptr<DelegatingSink> delegating_sink_;
};

} // namespace detail
} // namespace oxygen::log
