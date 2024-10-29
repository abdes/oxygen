//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>

#include "oxygen/platform/types.h"

#include "oxygen/input/action_triggers.h"
#include "oxygen/input/action_value.h"
#include "oxygen/input/types.h"

namespace oxygen::input {

class InputActionMapping {
public:
  InputActionMapping(
      std::shared_ptr<Action> action, const platform::InputSlot &input_slot);

  [[nodiscard]] auto GetAction() const {
    return action_;
  }

  [[nodiscard]] auto GetSlot() const -> const auto & {
    return slot_;
  }

  void AddTrigger(std::shared_ptr<ActionTrigger> trigger);
  [[nodiscard]] auto GetTriggers() const -> const auto & {
    return triggers_;
  }

  void HandleInput(const platform::InputEvent &event);
  void CancelInput();
  [[nodiscard]] auto Update(Duration delta_time) -> bool;

private:
  [[nodiscard]] auto DoUpdate(Duration delta_time) -> bool;

  void StartEvaluation();
  void CompleteEvaluation();
  void NotifyActionCanceled();
  void NotifyActionTriggered();
  void NotifyActionOngoing();

  std::shared_ptr<Action> action_;
  const platform::InputSlot &slot_;
  std::vector<std::shared_ptr<ActionTrigger>> triggers_;

  ActionValue action_value_;
  ActionValue last_action_value_;
  bool evaluation_ongoing_{false};
  bool event_processing_{false};
  bool found_explicit_trigger_{false};
  bool any_explicit_triggered_{false};
  bool any_explicit_ongoing_{false};
  bool all_implicits_triggered_{true};
  bool trigger_ongoing_{false};
  bool blocked_{false};
  bool action_ongoing_{false};
  bool clear_value_after_update_{false};
};

} // namespace oxygen::input

// TODO(abdes) addShouldBeIgnored flag
// If true, then this Key Mapping should be ignored.

// TODO(abdes) add modifiers to input action mapping.
// Modifiers applied, sequentially in the order they have been added, to the raw
// key value.