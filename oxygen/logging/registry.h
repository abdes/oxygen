//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <string> // for std::string

#include "spdlog/spdlog.h"

#include "oxygen/base/macros.h"

namespace oxygen::log {

namespace detail {
class RegistryImpl;
} // namespace detail

/*!
 * @brief A registry of all named loggers and the single point of access to the
 * logging API.
 *
 * The logging registry creates and manages all the named loggers in the
 * application. It can be used to:
 *   - obtain any registered logger by its name,
 *   - set logging level for all registered loggers,
 *   - change the logging format,
 *   - manage a stack of sinks where the current sink can be temporarily
 *     swapped with another sink, to be restored later
 *
 * The Registry creates a default sink at startup to be used by all registered
 * loggers, until an explicit call to PushSink() is made. The default sink is
 * a console logger (color).
 *
 * Example:
 * ```
 * {
 *   auto &logger =
 *     asap::logging::Registry::GetLogger("testing");
 *   ASLOG_TO_LOGGER(debug, "starting...");
 *
 *   // Initialize a complex GUI system
 *   ...
 *
 *   // Start logging to the graphical console
 *   auto ui_console = std::make_shared<MyCustomSink>();
 *   asap::logging::Registry::PushSink(ui_console);
 *
 *   ...
 *
 *   // Shutdown the GUI, switch back to the primitive logging sink
 *   asap::logging::Registry::PopSink()
 * ```
 *
 * @todo TODO: Add Init() method with format, sinks and level
 */
class Registry {
public:
  static auto Instance() -> Registry &;

  ~Registry() = default;

  OXYGEN_MAKE_NON_COPYABLE(Registry);
  OXYGEN_DEFAULT_MOVABLE(Registry);

  /*!
   * @brief Sets the minimum log severity required to print messages.
   *
   * Messages below this loglevel will be suppressed.
   *
   * @param [in] log_level the logging severity to be applied to all registered
   * loggers.
   */
  void SetLogLevel(spdlog::level::level_enum log_level) const;

  /*!
   * @brief Change the format string used by the registered loggers.
   *
   * @param [in] log_format new format string.
   *
   * @see https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
   */
  void SetLogFormat(const std::string &log_format) const;

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
  [[nodiscard]] auto GetLogger(
      std::string const &name) const -> spdlog::logger &;

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
  void PushSink(spdlog::sink_ptr sink) const;

  /*!
   * @brief Restore the top of sinks stack as the current sink for all
   * registered loggers.
   *
   * @see PushSink(spdlog::sink_ptr)
   */
  void PopSink() const;

private:
  Registry();

  std::unique_ptr<detail::RegistryImpl> pimpl_;
};

} // namespace oxygen::log
