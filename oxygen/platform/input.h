//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "oxygen/platform/types.h"

//------------------------------------------------------------------------------
// InputSlotDetails
//------------------------------------------------------------------------------

namespace oxygen::platform::detail {
class InputSlotDetails;
} // namespace oxygen::platform::detail

//------------------------------------------------------------------------------
// InputSlot
//------------------------------------------------------------------------------

namespace oxygen::platform {
class InputSlot {
public:
  explicit InputSlot(const std::string_view name) : name_(name) {
  }

  [[nodiscard]] auto GetName() const {
    return name_;
  }

  auto IsModifierKey() const -> bool;
  auto IsKeyboardKey() const -> bool;
  auto IsMouseButton() const -> bool;

  auto IsAxis1D() const -> bool;
  auto IsAxis2D() const -> bool;
  auto IsAxis3D() const -> bool;

  friend class InputSlots;

  [[nodiscard]] auto GetDisplayString() const -> std::string_view;

  [[nodiscard]] auto GetInputCategoryName() const -> std::string_view;

  friend auto operator==(const InputSlot &lhs, const InputSlot &rhs) -> bool {
    return lhs.name_ == rhs.name_;
  }
  friend auto operator!=(const InputSlot &lhs, const InputSlot &rhs) -> bool {
    return lhs.name_ != rhs.name_;
  }
  friend auto operator<(const InputSlot &lhs, const InputSlot &rhs) -> bool {
    return lhs.name_ < rhs.name_;
  }

private:
  std::string_view name_;
  mutable std::shared_ptr<detail::InputSlotDetails> details_;

  void UpdateDetailsIfNotUpdated() const;
};
} // namespace oxygen::platform

template <> struct std::hash<oxygen::platform::InputSlot> {
  auto operator()(
      const oxygen::platform::InputSlot &slot) const noexcept -> size_t {
    return hash<std::string_view>()(slot.GetName());
  }
};

//------------------------------------------------------------------------------
// InputSlots
//------------------------------------------------------------------------------

namespace oxygen::platform {
class InputSlots {
public:
  // Category names static string_view literals
  static const std::string_view kKeyCategoryName;
  static const std::string_view kMouseCategoryName;

  // -- Static input slots
  // NOLINTBEGIN
  // Mouse slots
  static const InputSlot MouseWheelUp;
  static const InputSlot MouseWheelDown;
  static const InputSlot MouseWheelLeft;
  static const InputSlot MouseWheelRight;
  static const InputSlot MouseWheelX;
  static const InputSlot MouseWheelY;
  static const InputSlot MouseWheelXY;
  static const InputSlot LeftMouseButton;
  static const InputSlot RightMouseButton;
  static const InputSlot MiddleMouseButton;
  static const InputSlot ThumbMouseButton1;
  static const InputSlot ThumbMouseButton2;
  static const InputSlot MouseX;
  static const InputSlot MouseY;
  static const InputSlot MouseXY;

  // Keyboard slots
  static const InputSlot None;
  static const InputSlot AnyKey;
  static const InputSlot BackSpace;
  static const InputSlot Delete;
  static const InputSlot Tab;
  static const InputSlot Clear;
  static const InputSlot Return;
  static const InputSlot Pause;
  static const InputSlot Escape;
  static const InputSlot Space;
  static const InputSlot Keypad0;
  static const InputSlot Keypad1;
  static const InputSlot Keypad2;
  static const InputSlot Keypad3;
  static const InputSlot Keypad4;
  static const InputSlot Keypad5;
  static const InputSlot Keypad6;
  static const InputSlot Keypad7;
  static const InputSlot Keypad8;
  static const InputSlot Keypad9;
  static const InputSlot KeypadPeriod;
  static const InputSlot KeypadDivide;
  static const InputSlot KeypadMultiply;
  static const InputSlot KeypadMinus;
  static const InputSlot KeypadPlus;
  static const InputSlot KeypadEnter;
  static const InputSlot KeypadEquals;
  static const InputSlot UpArrow;
  static const InputSlot DownArrow;
  static const InputSlot RightArrow;
  static const InputSlot LeftArrow;
  static const InputSlot Insert;
  static const InputSlot Home;
  static const InputSlot End;
  static const InputSlot PageUp;
  static const InputSlot PageDown;
  static const InputSlot F1;
  static const InputSlot F2;
  static const InputSlot F3;
  static const InputSlot F4;
  static const InputSlot F5;
  static const InputSlot F6;
  static const InputSlot F7;
  static const InputSlot F8;
  static const InputSlot F9;
  static const InputSlot F10;
  static const InputSlot F11;
  static const InputSlot F12;
  static const InputSlot F13;
  static const InputSlot F14;
  static const InputSlot F15;
  static const InputSlot Alpha0;
  static const InputSlot Alpha1;
  static const InputSlot Alpha2;
  static const InputSlot Alpha3;
  static const InputSlot Alpha4;
  static const InputSlot Alpha5;
  static const InputSlot Alpha6;
  static const InputSlot Alpha7;
  static const InputSlot Alpha8;
  static const InputSlot Alpha9;
  static const InputSlot Exclaim;
  static const InputSlot DoubleQuote;
  static const InputSlot Hash;
  static const InputSlot Dollar;
  static const InputSlot Percent;
  static const InputSlot Ampersand;
  static const InputSlot Quote;
  static const InputSlot LeftParen;
  static const InputSlot RightParen;
  static const InputSlot Asterisk;
  static const InputSlot Plus;
  static const InputSlot Comma;
  static const InputSlot Minus;
  static const InputSlot Period;
  static const InputSlot Slash;
  static const InputSlot Colon;
  static const InputSlot Semicolon;
  static const InputSlot Less;
  static const InputSlot Equals;
  static const InputSlot Greater;
  static const InputSlot Question;
  static const InputSlot At;
  static const InputSlot LeftBracket;
  static const InputSlot Backslash;
  static const InputSlot RightBracket;
  static const InputSlot Caret;
  static const InputSlot Underscore;
  static const InputSlot BackQuote;
  static const InputSlot A;
  static const InputSlot B;
  static const InputSlot C;
  static const InputSlot D;
  static const InputSlot E;
  static const InputSlot F;
  static const InputSlot G;
  static const InputSlot H;
  static const InputSlot I;
  static const InputSlot J;
  static const InputSlot K;
  static const InputSlot L;
  static const InputSlot M;
  static const InputSlot N;
  static const InputSlot O;
  static const InputSlot P;
  static const InputSlot Q;
  static const InputSlot R;
  static const InputSlot S;
  static const InputSlot T;
  static const InputSlot U;
  static const InputSlot V;
  static const InputSlot W;
  static const InputSlot X;
  static const InputSlot Y;
  static const InputSlot Z;
  static const InputSlot NumLock;
  static const InputSlot CapsLock;
  static const InputSlot ScrollLock;
  static const InputSlot RightShift;
  static const InputSlot LeftShift;
  static const InputSlot RightControl;
  static const InputSlot LeftControl;
  static const InputSlot RightAlt;
  static const InputSlot LeftAlt;
  static const InputSlot LeftMeta;
  static const InputSlot RightMeta;
  static const InputSlot Help;
  static const InputSlot Print;
  static const InputSlot SysReq;
  static const InputSlot Menu;
  // Mouse buttons
  // NOLINTEND

  friend class oxygen::Platform;
  friend class InputSlot;

  // TODO: review visibility and keep safe methods public
  // private:
  static void Initialize();

  static void GetAllInputSlots(std::vector<InputSlot> &out_keys);
  static auto GetInputSlotForKey(Key key) -> InputSlot;

  static auto GetCategoryDisplayName(
      std::string_view category_name) -> std::string_view;

  struct CategoryInfo {
    std::string_view display_string;
  };

  static std::map<InputSlot, std::shared_ptr<detail::InputSlotDetails>> slots_;
  static std::map<Key, InputSlot> key_slots_;
  static std::map<std::string_view, CategoryInfo> categories_;

  // TODO(abdes) add user defined slots and categories
  static void AddCategory(
      std::string_view category_name, std::string_view display_string);
  static void AddInputSlot(const detail::InputSlotDetails &detail);
  static void AddKeyInputSlot(
      Key key_code, const detail::InputSlotDetails &detail);
  [[nodiscard]] static auto GetInputSlotDetails(
      const InputSlot &slot) -> std::shared_ptr<detail::InputSlotDetails>;
};

} // namespace oxygen::platform
