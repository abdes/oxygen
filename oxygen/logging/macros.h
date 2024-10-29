//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include "spdlog/spdlog.h"

#include "registry.h"

// ---------------------------------------------------------------------------
// Loggable
// ---------------------------------------------------------------------------

namespace oxygen::log {

/*!
 * @brief Mixin class that allows any class to perform logging with a logger of
 * a particular name.
 */
template <typename T> class Loggable {
protected:
  /*!
   * @brief Do not use this directly, use macros defined below.
   * @return spdlog::logger& the static log instance to use for class local
   * logging.
   */
  static auto internal_log_do_not_use_read_comment() -> spdlog::logger & {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    static spdlog::logger &instance =
        Registry::Instance().GetLogger(T::LOGGER_NAME);
    return instance;
  }
};

namespace detail {

/*!
 * @brief A simple mapping between Logger severity levels and spdlog severity
 * levels.
 *
 * The only reason for this mapping is to go around the fact that spdlog
 * defines level as err but the method to log at err level is called
 * LOGGER.error not LOGGER.err. All other levels are fine.
 */
enum class Level : uint8_t {
  trace = spdlog::level::trace,
  debug = spdlog::level::debug,
  info = spdlog::level::info,
  warn = spdlog::level::warn,
  error = spdlog::level::err,
  critical = spdlog::level::critical,
  off = spdlog::level::off
};

} // namespace detail
} // namespace oxygen::log

// ---------------------------------------------------------------------------
// Internal Helper Macros
// ---------------------------------------------------------------------------

// Base logging macros. It is expected that users will use the
// convenience macros below rather than invoke these directly.
#if !defined(DOXYGEN_DOCUMENTATION_BUILD)

// Convert the line macro to a string literal for concatenation in log macros.
#define DO_STRINGIZE(x) STRINGIZE(x)
#define STRINGIZE(x) #x
#define LINE_STRING DO_STRINGIZE(__LINE__)
#ifndef NDEBUG
namespace oxygen::log::detail {
auto FormatFileAndLine(char const *file, char const *line) -> std::string;
}
#define LOG_PREFIX oxygen::log::detail::FormatFileAndLine(__FILE__, LINE_STRING)
#else
#define LOG_PREFIX " "
#endif // NDEBUG

#define ASLOG_COMP_LEVEL(LOGGER, LEVEL)                                        \
  (static_cast<std::underlying_type_t<oxygen::log::detail::Level>>(            \
       oxygen::log::detail::Level::LEVEL) >= ((LOGGER).level()))

// Compare levels before invoking logger. This is an optimization to avoid
// executing expressions computing log contents when they would be suppressed.
// The same filtering will also occur in spdlog::logger.

#define INTERNAL_SELECT(PREFIX, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4,    \
    _3, _2, _1, SUFFIX, ...)                                                   \
  PREFIX##_##SUFFIX
#define INTERNAL_SELECT_IMPL(args) INTERNAL_SELECT args
#define AS_DO_LOG(...)                                                         \
  INTERNAL_SELECT_IMPL(                                                        \
      (INTERNAL_ASLOG, __VA_ARGS__, N, N, N, N, N, N, N, N, N, N, 3, 2, 1))    \
  (__VA_ARGS__)

#define INTERNAL_ASLOG_1(LOGGER) LOGGER.debug("no logger level - no message")
#define INTERNAL_ASLOG_2(LOGGER, LEVEL) LOGGER.LEVEL("no message")
#define INTERNAL_ASLOG_3(LOGGER, LEVEL, MSG) LOGGER.LEVEL("{}" MSG, LOG_PREFIX)
#define INTERNAL_ASLOG_N(LOGGER, LEVEL, MSG, ...)                              \
  LOGGER.LEVEL("{}" MSG, LOG_PREFIX, __VA_ARGS__)

#ifndef NDEBUG
#define ASLOG_COMP_AND_LOG(LOGGER, LEVEL, ...)                                 \
  if (ASLOG_COMP_LEVEL(LOGGER, LEVEL)) {                                       \
    AS_DO_LOG(LOGGER, LEVEL, __VA_ARGS__);                                     \
  }
#else // NDEBUG
#define ASLOG_COMP_AND_LOG(LOGGER, LEVEL, ...)                                 \
  if (ASLOG_COMP_LEVEL(LOGGER, LEVEL)) {                                       \
    LOGGER.LEVEL(__VA_ARGS__);                                                 \
  }
#endif // NDEBUG

#define GET_MISC_LOGGER() oxygen::log::Registry::Instance().GetLogger("misc")

#endif // DOXYGEN_DOCUMENTATION_BUILD

// ---------------------------------------------------------------------------
// User Convenience Macros
// ---------------------------------------------------------------------------

/// @name Logging macros
//@{

/**
 * Check the class logger level against the provided one. Resolves to true if
 * the class logger level is <= to the given level.
 */
#define ASLOG_CHECK_LEVEL(LEVEL) ASLOG_COMP_LEVEL(ASLOGGER(), LEVEL)

/**
 * Convenience macro to get the class logger.
 */
#define ASLOGGER() internal_log_do_not_use_read_comment()

/**
 * Convenience macro to log to the class logger.
 */
#define ASLOG(LEVEL, ...) ASLOG_TO_LOGGER(ASLOGGER(), LEVEL, __VA_ARGS__)

/**
 * Convenience macro to flush logger.
 */
#define ASFLUSH_LOG() ASLOGGER().flush()

/**
 * Convenience macro to log to a user-specified logger.
 */
#define ASLOG_TO_LOGGER(LOGGER, LEVEL, ...)                                    \
  ASLOG_COMP_AND_LOG(LOGGER, LEVEL, __VA_ARGS__)

/**
 * Convenience macro to log to the misc logger, which allows for logging without
 * direct access to a logger.
 */
#define ASLOG_MISC(LEVEL, ...)                                                 \
  ASLOG_TO_LOGGER(GET_MISC_LOGGER(), LEVEL, __VA_ARGS__)

/**
 * Convenience macro to print a log to a user-specified logger but only if DEBUG
 * is defined.
 */
#ifndef NDEBUG
#define ASDEBUG_TO_LOGGER(LOGGER, ...)                                         \
  ASLOG_TO_LOGGER(LOGGER, debug, __VA_ARGS__)
#else
#define ASDEBUG_TO_LOGGER(LOGGER, ...) void(0)
#endif

/**
 * Convenience macro to log to the class logger but only if DEBUG is defined.
 */
#ifndef NDEBUG
#define ASDEBUG(...) ASLOG(debug, __VA_ARGS__)
#else
#define ASDEBUG(...) void(0)
#endif

//@}
