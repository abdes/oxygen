//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <chrono>
#include <concepts>

#include "oxygen/base/types.h"

namespace oxygen {

struct Time
{
  static auto Now() -> TimePoint
  {
    static const auto kLocalEpoch = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<TimePoint>(
        std::chrono::high_resolution_clock::now() - kLocalEpoch);
  }
};

template <typename T>
concept has_now_method = requires(T) {
  { T::Now() } -> std::same_as<TimePoint>;
};

template <has_now_method T>
class ElapsedTimeType
{
 public:
  [[nodiscard]] auto StartTime() const -> auto const& { return start_time_; }
  [[nodiscard]] auto ElapsedTime() const -> auto
  {
    return T::Now() - start_time_;
  }

 private:
  TimePoint start_time_{T::Now()};
};

using ElapsedTimeCounter = ElapsedTimeType<Time>;

template <has_now_method T>
class DeltaTimeType
{
 public:
  auto Update() -> void
  {
    auto now = T::Now();
    delta_ = now - last_step_time_;
    last_step_time_ = now;
  }

  auto Reset() -> void
  {
    last_step_time_ = T::Now();
    delta_ = Duration::zero();
  }

  [[nodiscard]] auto LastStepTime() const -> auto { return last_step_time_; }

  [[nodiscard]] auto Delta() const -> auto { return delta_; }

 private:
  TimePoint last_step_time_{T::Now()};
  Duration delta_{};
};

using DeltaTimeCounter = DeltaTimeType<Time>;

template <has_now_method T>
class ChangePerSecondType
{
 public:
  auto Update() -> void
  {
    ++temp_value_;
    auto now = T::Now();
    if (std::chrono::floor<std::chrono::seconds>(
            std::chrono::duration_cast<std::chrono::seconds>(now)) >
        std::chrono::floor<std ::chrono::seconds>(
            std::chrono::duration_cast<std::chrono::seconds>(value_time_))) {
      value_ = temp_value_;
      temp_value_ = 0;
    }
    value_time_ = now;
  }

  [[nodiscard]] auto Value() const -> auto { return value_; }

  [[nodiscard]] auto ValueTime() const -> auto { return value_time_; }

 private:
  uint32_t temp_value_{};
  uint32_t value_{};
  TimePoint value_time_{T::Now()};
};

using ChangePerSecondCounter = ChangePerSecondType<Time>;

}  // namespace oxygen
