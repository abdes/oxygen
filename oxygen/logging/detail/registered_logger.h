//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <algorithm>
#include <string>

#include "spdlog/spdlog.h"

namespace oxygen::log::detail {

/**
 * @brief Logger wrapper for a spdlog logger.
 */
class Logger final {
public:
  // Non-copiable
  Logger(const Logger &) = delete;
  // Non-assignable
  auto operator=(const Logger &) -> Logger & = delete;

  /// Move constructor
  Logger(Logger &&other) noexcept
      : logger_(std::move(other.logger_)),
        logger_mutex_(std::move(other.logger_mutex_)) {
  }

  /// Move assignment
  auto operator=(Logger &&other) noexcept -> Logger & {
    Logger(std::move(other)).swap(*this);
    return *this;
  }

  /// Default trivial destructor
  ~Logger() = default;

  /*!
   * @brief Implementation of the swap operation.
   *
   * @param other Logger object to swap with.
   */
  void swap(Logger &other) noexcept {
    std::swap(logger_, other.logger_);
  }

  /*!
   * @brief Get this logger's name.
   *
   * @return the logger name.
   */
  [[nodiscard]] auto Name() const -> const std::string & {
    return logger_->name();
  }

  /*!
   * @brief Set the logging level for this logger (e.g. debug, warning...).
   *
   * @param [in] level logging level.
   */
  void SetLevel(const spdlog::level::level_enum level) const {
    logger_->set_level(level);
  }

  /*!
   * @brief Get the logging level for this logger.
   *
   * @return This logger's logging level.
   */
  [[nodiscard]] auto GetLevel() const -> spdlog::level::level_enum {
    return logger_->level();
  }

private:
  /*!
   * @brief Create a Logger with the given name and make it use the given sink
   * for its log messages.
   *
   * Logger objects cannot be created directly. Instead, use the Registry class
   * to obtain a Logger for a specific name.
   *
   * In spdlog, loggers get assigned a sink or several sinks only at creation
   * and have to continue using that sink for the rest of their lifetime.
   * The Registry class offer an extension feature that allows to dynamically
   * switch the current sink. See Registry::PushSink() and Registry::PopSink().
   *
   * @param [in] name the logger name.
   * @param [in] sink the sink to be used by this logger.
   *
   * @see Registry::GetLogger(std::string)
   */
  Logger(const std::string &name, const spdlog::sink_ptr &sink);

  /// The underlying spdlog::logger instance.
  std::shared_ptr<spdlog::logger> logger_;
  /// Synchronization lock used to synchronize logging over this logger from
  /// multiple threads.
  std::unique_ptr<std::mutex> logger_mutex_;

  /// Logger objects are created only by the Registry class.
  friend class RegistryImpl;
};

} // namespace oxygen::log::detail
