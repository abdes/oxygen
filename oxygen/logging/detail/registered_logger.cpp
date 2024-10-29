//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "registered_logger.h"

#include <memory>
#include <string>

oxygen::log::detail::Logger::Logger(
    const std::string &name, const spdlog::sink_ptr &sink)
    : logger_(std::make_shared<spdlog::logger>(name, sink)),
      logger_mutex_(std::make_unique<std::mutex>()) {
  logger_->set_level(spdlog::level::trace);
  // Ensure that critical errors, especially ASSERT/PANIC, get flushed
  logger_->flush_on(spdlog::level::critical);
}
