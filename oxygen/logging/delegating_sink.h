//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include "spdlog/sinks/base_sink.h"

namespace oxygen::log {
/*!
 * @brief A logging sink implementation that delegates all its logging calls
 * to an encapsulated delegate.
 *
 * This class is used to work around the limitation of spdlog that forces the
 * same sink(s) to be used for the lifetime of a logger. Two important
 * application scenarios require the sink to be changed after the logger object
 * is created:
 *   - If the application starts logging early to console and then later needs
 *     to log to some different sink after the proper resources for that sink
 *     have been initialized (e.g. GUI),
 *   - If the application needs to temporarily switch logging output to a
 *     different sink (e.g. dumping diagnostic data) and then switch back to
 *     original sink after it's done. This requires two loggers in the current
 *     implementation.
 *
 * The DelegatingSink class supports switching its delegate at any time.
 */
class DelegatingSink : public spdlog::sinks::base_sink<std::mutex> {
public:
  /*!
   * @brief Create a DelegatingSink which delegates all its logging to the given
   * delegate sink.
   *
   * @param [in] delegate the sink to which logging calls will be delegated.
   */
  explicit DelegatingSink(spdlog::sink_ptr delegate)
      : sink_delegate_(std::move(delegate)) {
  }

  // Non-copyable
  DelegatingSink(const DelegatingSink &) = delete;
  // Non-assignable
  auto operator=(const DelegatingSink &) -> DelegatingSink & = delete;

  /// Move constructor
  DelegatingSink(DelegatingSink &&) = delete;

  /// Move assignment
  auto operator=(DelegatingSink &&) -> DelegatingSink & = delete;

  /// Default trivial destructor
  DelegatingSink() = default;

  /// Default trivial destructor
  ~DelegatingSink() override = default;

  /*!
   * @brief Use the given sink as a new delegate and return the old one.
   *
   * @param new_sink the new delegate.
   * @return the previously used delegate.
   */
  auto SwapSink(spdlog::sink_ptr new_sink) -> spdlog::sink_ptr {
    std::lock_guard<std::mutex> lock(mutex_);
    auto tmp = sink_delegate_;
    sink_delegate_ = std::move(new_sink);
    return tmp;
  }

protected:
  /// @name base_sink interface
  //@{
  /*!
   * @brief Process the given log message.
   *
   * @param msg log message to be processed.
   */
  void sink_it_(const spdlog::details::log_msg &msg) override {
    sink_delegate_->log(msg);
  }

  /// Called when this sink needs to flush any buffered log messages.
  void flush_() override {
    sink_delegate_->flush();
  }

  void set_pattern_(const std::string &pattern) override {
    sink_delegate_->set_pattern(pattern);
  }

  void set_formatter_(
      std::unique_ptr<spdlog::formatter> sink_formatter) override {
    sink_delegate_->set_formatter(std::move(sink_formatter));
  }
  //@}

private:
  /// The delegate sink.
  spdlog::sink_ptr sink_delegate_;
};

} // namespace oxygen::log
