//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "oxygen/platform/input.h"

#include <cassert>
#include <memory>
#include <ranges>
#include <string_view>
#include <utility>

#include "oxygen/logging/logging.h"

namespace {
auto &platform_logger = // NOLINT(*-avoid-non-const-global-variables)
    oxygen::log::Registry::Instance().GetLogger("Oxygen.Platform");
}

//------------------------------------------------------------------------------
// InputSlotDetails
//------------------------------------------------------------------------------

namespace oxygen::platform::detail {

class InputSlotDetails {
public:
  enum class Flags {
    kNone = 0,

    kMouseButton = 1 << 0,
    kKeyboardKey = 1 << 1,
    kModifierKey = 1 << 2,

    kAxis1D = 1 << 16,
    kAxis2D = 1 << 17,
    kAxis3D = 1 << 18,
  };

  InputSlotDetails(const InputSlot &slot, std::string_view display_string,
      Flags flags = Flags::kNone, std::string_view category_name = {});
  ~InputSlotDetails() = default;
  // Non-copyable
  InputSlotDetails(const InputSlotDetails &) = default;
  auto operator=(const InputSlotDetails &) -> InputSlotDetails & = delete;

  // Non-Movable
  InputSlotDetails(InputSlotDetails &&other) noexcept = default;
  auto operator=(
      InputSlotDetails &&other) noexcept -> InputSlotDetails & = delete;

  [[nodiscard]] auto GetSlot() const -> const auto & {
    return slot_;
  }

  // Informational details about the input slot, will be used in the editor for
  // a user-friendly presentation of the different slots and slot categories.
  // Should not be used at runtime where it is preferred to rely on the input
  // event type to obtain the relevant embedded values in the event.

  [[nodiscard]] auto GetDisplayString() const {
    return display_string_;
  }

  [[nodiscard]] auto GetInputCategoryName() const {
    return category_name_;
  }
  [[nodiscard]] auto IsMouseButton() const {
    return is_mouse_button_ != 0;
  }
  [[nodiscard]] auto IsKeyboardKey() const {
    return is_keyboard_key_ != 0;
  }
  [[nodiscard]] auto IsModifierKey() const {
    return is_modifier_key_ != 0;
  }

  [[nodiscard]] auto IsAxis1D() const {
    return is_axis_1d_ != 0;
  }
  [[nodiscard]] auto IsAxis2D() const {
    return is_axis_2d_ != 0;
  }
  [[nodiscard]] auto IsAxis3D() const {
    return is_axis_3d_ != 0;
  }

  friend InputSlots;

private:
  // The slots are all initialized and owned globally by the platform. Only
  // references to the pre-initialized slots are used, which will always be
  // valid.
  const InputSlot &slot_; // NOLINT(*-avoid-const-or-ref-data-members)

  std::string_view display_string_;
  std::string_view category_name_;

  uint8_t is_keyboard_key_ : 1 {0};
  uint8_t is_modifier_key_ : 1 {0};
  uint8_t is_mouse_button_ : 1 {0};

  uint8_t is_axis_1d_ : 1 {0};
  uint8_t is_axis_2d_ : 1 {0};
  uint8_t is_axis_3d_ : 1 {0};
};

constexpr auto operator|=(InputSlotDetails::Flags &mods,
    const InputSlotDetails::Flags other) -> auto & {
  mods = static_cast<InputSlotDetails::Flags>(
      static_cast<std::underlying_type_t<InputSlotDetails::Flags>>(mods) |
      static_cast<std::underlying_type_t<InputSlotDetails::Flags>>(other));
  return mods;
}

constexpr auto operator|(
    const InputSlotDetails::Flags left, const InputSlotDetails::Flags right) {
  const auto mods = static_cast<InputSlotDetails::Flags>(
      static_cast<std::underlying_type_t<InputSlotDetails::Flags>>(left) |
      static_cast<std::underlying_type_t<InputSlotDetails::Flags>>(right));
  return mods;
}

constexpr auto operator&(
    const InputSlotDetails::Flags left, const InputSlotDetails::Flags right) {
  const auto mods = static_cast<InputSlotDetails::Flags>(
      static_cast<std::underlying_type_t<InputSlotDetails::Flags>>(left) &
      static_cast<std::underlying_type_t<InputSlotDetails::Flags>>(right));
  return mods;
}

} // namespace oxygen::platform::detail

using oxygen::platform::InputSlot;
using oxygen::platform::InputSlots;
using oxygen::platform::detail::InputSlotDetails;

// NOLINTBEGIN
const std::string_view InputSlots::kMouseCategoryName = "Mouse";
const std::string_view InputSlots::kKeyCategoryName = "Key";

// Mouse slots
const InputSlot InputSlots::MouseWheelUp("MouseWheelUp");
const InputSlot InputSlots::MouseWheelDown("MouseWheelDown");
const InputSlot InputSlots::MouseWheelLeft("MouseWheelLeft");
const InputSlot InputSlots::MouseWheelRight("MouseWheelRight");
const InputSlot InputSlots::MouseWheelX("MouseWheelX");
const InputSlot InputSlots::MouseWheelY("MouseWheelY");
const InputSlot InputSlots::MouseWheelXY("MouseWheelXY");
const InputSlot InputSlots::LeftMouseButton("LeftMouseButton");
const InputSlot InputSlots::RightMouseButton("RightMouseButton");
const InputSlot InputSlots::MiddleMouseButton("MiddleMouseButton");
const InputSlot InputSlots::ThumbMouseButton1("ThumbMouseButton1");
const InputSlot InputSlots::ThumbMouseButton2("ThumbMouseButton2");
const InputSlot InputSlots::MouseX("MouseX");
const InputSlot InputSlots::MouseY("MouseY");
const InputSlot InputSlots::MouseXY("MouseXY");

// Keyboard slots
const InputSlot InputSlots::None("None");
const InputSlot InputSlots::AnyKey("AnyKey");

const InputSlot InputSlots::BackSpace("BackSpace");
const InputSlot InputSlots::Delete("Delete");
const InputSlot InputSlots::Tab("Tab");
const InputSlot InputSlots::Clear("Clear");
const InputSlot InputSlots::Return("Return");
const InputSlot InputSlots::Pause("Pause");
const InputSlot InputSlots::Escape("Escape");
const InputSlot InputSlots::Space("Space");
const InputSlot InputSlots::Keypad0("Keypad0");
const InputSlot InputSlots::Keypad1("Keypad1");
const InputSlot InputSlots::Keypad2("Keypad2");
const InputSlot InputSlots::Keypad3("Keypad3");
const InputSlot InputSlots::Keypad4("Keypad4");
const InputSlot InputSlots::Keypad5("Keypad5");
const InputSlot InputSlots::Keypad6("Keypad6");
const InputSlot InputSlots::Keypad7("Keypad7");
const InputSlot InputSlots::Keypad8("Keypad8");
const InputSlot InputSlots::Keypad9("Keypad9");
const InputSlot InputSlots::KeypadPeriod("KeypadPeriod");
const InputSlot InputSlots::KeypadDivide("KeypadDivide");
const InputSlot InputSlots::KeypadMultiply("KeypadMultiply");
const InputSlot InputSlots::KeypadMinus("KeypadMinus");
const InputSlot InputSlots::KeypadPlus("KeypadPlus");
const InputSlot InputSlots::KeypadEnter("KeypadEnter");
const InputSlot InputSlots::KeypadEquals("KeypadEquals");
const InputSlot InputSlots::UpArrow("Up");
const InputSlot InputSlots::DownArrow("Down");
const InputSlot InputSlots::RightArrow("Right");
const InputSlot InputSlots::LeftArrow("Left");
const InputSlot InputSlots::Insert("Insert");
const InputSlot InputSlots::Home("Home");
const InputSlot InputSlots::End("End");
const InputSlot InputSlots::PageUp("PageUp");
const InputSlot InputSlots::PageDown("PageDown");
const InputSlot InputSlots::F1("F1");
const InputSlot InputSlots::F2("F2");
const InputSlot InputSlots::F3("F3");
const InputSlot InputSlots::F4("F4");
const InputSlot InputSlots::F5("F5");
const InputSlot InputSlots::F6("F6");
const InputSlot InputSlots::F7("F7");
const InputSlot InputSlots::F8("F8");
const InputSlot InputSlots::F9("F9");
const InputSlot InputSlots::F10("F10");
const InputSlot InputSlots::F11("F11");
const InputSlot InputSlots::F12("F12");
const InputSlot InputSlots::F13("F13");
const InputSlot InputSlots::F14("F14");
const InputSlot InputSlots::F15("F15");
const InputSlot InputSlots::Alpha0("0");
const InputSlot InputSlots::Alpha1("1");
const InputSlot InputSlots::Alpha2("2");
const InputSlot InputSlots::Alpha3("3");
const InputSlot InputSlots::Alpha4("4");
const InputSlot InputSlots::Alpha5("5");
const InputSlot InputSlots::Alpha6("6");
const InputSlot InputSlots::Alpha7("7");
const InputSlot InputSlots::Alpha8("8");
const InputSlot InputSlots::Alpha9("9");
const InputSlot InputSlots::Exclaim("!");
const InputSlot InputSlots::DoubleQuote("DoubleQuote");
const InputSlot InputSlots::Hash("Hash");
const InputSlot InputSlots::Dollar("Dollar");
const InputSlot InputSlots::Percent("Percent");
const InputSlot InputSlots::Ampersand("Ampersand");
const InputSlot InputSlots::Quote("Quote");
const InputSlot InputSlots::LeftParen("LeftParen");
const InputSlot InputSlots::RightParen("RightParen");
const InputSlot InputSlots::Asterisk("Asterisk");
const InputSlot InputSlots::Plus("Plus");
const InputSlot InputSlots::Comma("Comma");
const InputSlot InputSlots::Minus("Minus");
const InputSlot InputSlots::Period("Period");
const InputSlot InputSlots::Slash("Slash");
const InputSlot InputSlots::Colon("Colon");
const InputSlot InputSlots::Semicolon("Semicolon");
const InputSlot InputSlots::Less("Less");
const InputSlot InputSlots::Equals("Equals");
const InputSlot InputSlots::Greater("Greater");
const InputSlot InputSlots::Question("Question");
const InputSlot InputSlots::At("At");
const InputSlot InputSlots::LeftBracket("LeftBracket");
const InputSlot InputSlots::Backslash("Backslash");
const InputSlot InputSlots::RightBracket("RightBracket");
const InputSlot InputSlots::Caret("Caret");
const InputSlot InputSlots::Underscore("Underscore");
const InputSlot InputSlots::BackQuote("BackQuote");
const InputSlot InputSlots::A("A");
const InputSlot InputSlots::B("B");
const InputSlot InputSlots::C("C");
const InputSlot InputSlots::D("D");
const InputSlot InputSlots::E("E");
const InputSlot InputSlots::F("F");
const InputSlot InputSlots::G("G");
const InputSlot InputSlots::H("H");
const InputSlot InputSlots::I("I");
const InputSlot InputSlots::J("J");
const InputSlot InputSlots::K("K");
const InputSlot InputSlots::L("L");
const InputSlot InputSlots::M("M");
const InputSlot InputSlots::N("N");
const InputSlot InputSlots::O("O");
const InputSlot InputSlots::P("P");
const InputSlot InputSlots::Q("Q");
const InputSlot InputSlots::R("R");
const InputSlot InputSlots::S("S");
const InputSlot InputSlots::T("T");
const InputSlot InputSlots::U("U");
const InputSlot InputSlots::V("V");
const InputSlot InputSlots::W("W");
const InputSlot InputSlots::X("X");
const InputSlot InputSlots::Y("Y");
const InputSlot InputSlots::Z("Z");
const InputSlot InputSlots::NumLock("NumLock");
const InputSlot InputSlots::CapsLock("CapsLock");
const InputSlot InputSlots::ScrollLock("ScrollLock");
const InputSlot InputSlots::RightShift("RightShift");
const InputSlot InputSlots::LeftShift("LeftShift");
const InputSlot InputSlots::RightControl("RightCtrl");
const InputSlot InputSlots::LeftControl("LeftCtrl");
const InputSlot InputSlots::RightAlt("RightAlt");
const InputSlot InputSlots::LeftAlt("LeftAlt");
const InputSlot InputSlots::LeftMeta("LeftMeta");
const InputSlot InputSlots::RightMeta("RightMeta");
const InputSlot InputSlots::Help("Help");
const InputSlot InputSlots::Print("PrintScreen");
const InputSlot InputSlots::SysReq("SysReq");
const InputSlot InputSlots::Menu("Menu");
// NOLINTEND
// End - Keyboard slots

void InputSlot::UpdateDetailsIfNotUpdated() const {
  if (!details_) {
    details_ = InputSlots::GetInputSlotDetails(*this);
  }
}

auto InputSlot::IsModifierKey() const -> bool {
  UpdateDetailsIfNotUpdated();
  return details_->IsModifierKey();
}

auto InputSlot::IsKeyboardKey() const -> bool {
  UpdateDetailsIfNotUpdated();
  return details_->IsKeyboardKey();
}

auto InputSlot::IsMouseButton() const -> bool {
  UpdateDetailsIfNotUpdated();
  return details_->IsMouseButton();
}

auto InputSlot::IsAxis1D() const -> bool {
  UpdateDetailsIfNotUpdated();
  return details_->IsAxis1D();
}

auto InputSlot::IsAxis2D() const -> bool {
  UpdateDetailsIfNotUpdated();
  return details_->IsAxis2D();
}

auto InputSlot::IsAxis3D() const -> bool {
  UpdateDetailsIfNotUpdated();
  return details_->IsAxis3D();
}

auto InputSlot::GetDisplayString() const -> std::string_view {
  UpdateDetailsIfNotUpdated();
  return details_->GetDisplayString();
}

auto InputSlot::GetInputCategoryName() const -> std::string_view {
  UpdateDetailsIfNotUpdated();
  return details_->GetInputCategoryName();
}

InputSlotDetails::InputSlotDetails(const InputSlot &slot,
    const std::string_view display_string, const Flags flags,
    const std::string_view category_name)
    : slot_(slot), display_string_(display_string) {
  if ((flags & InputSlotDetails::Flags::kMouseButton) !=
      InputSlotDetails::Flags::kNone) {
    is_mouse_button_ = true;
    is_keyboard_key_ = false;
  }
  is_modifier_key_ = (flags & InputSlotDetails::Flags::kModifierKey) !=
                     InputSlotDetails::Flags::kNone;

  is_axis_1d_ = (flags & InputSlotDetails::Flags::kAxis1D) !=
                InputSlotDetails::Flags::kNone;
  is_axis_2d_ = (flags & InputSlotDetails::Flags::kAxis2D) !=
                InputSlotDetails::Flags::kNone;
  is_axis_3d_ = (flags & InputSlotDetails::Flags::kAxis3D) !=
                InputSlotDetails::Flags::kNone;

  // Set up default menu categories
  if (category_name.empty()) {
    if (IsMouseButton()) {
      category_name_ = InputSlots::kMouseCategoryName;
    } else {
      category_name_ = InputSlots::kKeyCategoryName;
    }
  }
}

// NOLINTBEGIN
std::map<InputSlot, std::shared_ptr<InputSlotDetails>> InputSlots::slots_;
std::map<oxygen::platform::Key, InputSlot> InputSlots::key_slots_;
std::map<std::string_view, InputSlots::CategoryInfo> InputSlots::categories_;
// NOLINTEND

void InputSlots::AddCategory(const std::string_view category_name,
    const std::string_view display_string) {
  if (categories_.contains(category_name)) {
    ASLOG_TO_LOGGER(platform_logger, warn,
        "Category with name [{}] has already been added.", category_name);
  }
  categories_[category_name] = CategoryInfo{.display_string = display_string};
}

void InputSlots::AddInputSlot(const InputSlotDetails &details) {
  const InputSlot &slot = details.GetSlot();
  assert(!slots_.contains(slot) && "slot already added");
  slot.details_ = std::make_shared<InputSlotDetails>(details);
  slots_[slot] = slot.details_;
}

void InputSlots::AddKeyInputSlot(
    Key key_code, const InputSlotDetails &details) {
  assert(!key_slots_.contains(key_code) && "key code already mapped");
  const InputSlot &slot = details.GetSlot();
  assert(!slots_.contains(slot) && "slot already added");
  key_slots_.insert({key_code, slot});
  slot.details_ = std::make_shared<InputSlotDetails>(details);
  slot.details_->is_keyboard_key_ = true;
  slot.details_->category_name_ = kKeyCategoryName;
  slots_[slot] = slot.details_;
}

void InputSlots::Initialize() {
  static bool initialized{false};

  if (initialized) {
    return;
  }
  initialized = true;

  ASLOG_TO_LOGGER(platform_logger, info, "Initializing the input slots");

  // clang-format off
  AddCategory(kKeyCategoryName, "Mouse");
  AddCategory(kMouseCategoryName, "Keyboard");

  // Mouse buttons
  AddInputSlot(InputSlotDetails(InputSlots::MouseX, "Mouse X", InputSlotDetails::Flags::kMouseButton | InputSlotDetails::Flags::kAxis1D));
  AddInputSlot(InputSlotDetails(InputSlots::MouseY, "Mouse Y", InputSlotDetails::Flags::kMouseButton | InputSlotDetails::Flags::kAxis1D));
  AddInputSlot(InputSlotDetails(InputSlots::MouseXY, "Mouse XY", InputSlotDetails::Flags::kMouseButton | InputSlotDetails::Flags::kAxis2D));
  AddInputSlot(InputSlotDetails(InputSlots::MouseWheelX, "Mouse Wheel X", InputSlotDetails::Flags::kMouseButton | InputSlotDetails::Flags::kAxis1D));
  AddInputSlot(InputSlotDetails(InputSlots::MouseWheelY, "Mouse Wheel Y", InputSlotDetails::Flags::kMouseButton | InputSlotDetails::Flags::kAxis1D));
  AddInputSlot(InputSlotDetails(InputSlots::MouseWheelXY, "Mouse Wheel XY", InputSlotDetails::Flags::kMouseButton | InputSlotDetails::Flags::kAxis2D));
  AddInputSlot(InputSlotDetails(InputSlots::MouseWheelUp, "Mouse Wheel Tick Up", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::MouseWheelDown, "Mouse Wheel Tick Down", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::MouseWheelLeft, "Mouse Wheel Tap Left", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::MouseWheelRight, "Mouse Wheel Tap Right", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::LeftMouseButton, "Left Mouse Button", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::RightMouseButton, "Right Mouse Button", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::MiddleMouseButton, "Middle Mouse Button", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::ThumbMouseButton1, "Thumb Mouse Button 1", InputSlotDetails::Flags::kMouseButton));
  AddInputSlot(InputSlotDetails(InputSlots::ThumbMouseButton2, "Thumb Mouse Button 2", InputSlotDetails::Flags::kMouseButton));

  // Keyboard keys
  AddInputSlot(InputSlotDetails(InputSlots::AnyKey, "Any Key"));

  AddKeyInputSlot(Key::kBackSpace, InputSlotDetails(InputSlots::BackSpace, "Back Space"));
  AddKeyInputSlot(Key::kDelete, InputSlotDetails(InputSlots::Delete, "Delete"));
  AddKeyInputSlot(Key::kTab, InputSlotDetails(InputSlots::Tab, "Tab"));
  AddKeyInputSlot(Key::kClear, InputSlotDetails(InputSlots::Clear, "Clear"));
  AddKeyInputSlot(Key::kReturn, InputSlotDetails(InputSlots::Return, "Return"));
  AddKeyInputSlot(Key::kPause, InputSlotDetails(InputSlots::Pause, "Pause"));
  AddKeyInputSlot(Key::kEscape, InputSlotDetails(InputSlots::Escape, "Escape"));
  AddKeyInputSlot(Key::kSpace, InputSlotDetails(InputSlots::Space, "Space"));
  AddKeyInputSlot(Key::kKeypad0, InputSlotDetails(InputSlots::Keypad0, "Keypad 0"));
  AddKeyInputSlot(Key::kKeypad1, InputSlotDetails(InputSlots::Keypad1, "Keypad 1"));
  AddKeyInputSlot(Key::kKeypad2, InputSlotDetails(InputSlots::Keypad2, "Keypad 2"));
  AddKeyInputSlot(Key::kKeypad3, InputSlotDetails(InputSlots::Keypad3, "Keypad 3"));
  AddKeyInputSlot(Key::kKeypad4, InputSlotDetails(InputSlots::Keypad4, "Keypad 4"));
  AddKeyInputSlot(Key::kKeypad5, InputSlotDetails(InputSlots::Keypad5, "Keypad 5"));
  AddKeyInputSlot(Key::kKeypad6, InputSlotDetails(InputSlots::Keypad6, "Keypad 6"));
  AddKeyInputSlot(Key::kKeypad7, InputSlotDetails(InputSlots::Keypad7, "Keypad 7"));
  AddKeyInputSlot(Key::kKeypad8, InputSlotDetails(InputSlots::Keypad8, "Keypad 8"));
  AddKeyInputSlot(Key::kKeypad9, InputSlotDetails(InputSlots::Keypad9, "Keypad 9"));
  AddKeyInputSlot(Key::kKeypadPeriod, InputSlotDetails(InputSlots::KeypadPeriod, "Keypad ."));
  AddKeyInputSlot(Key::kKeypadDivide, InputSlotDetails(InputSlots::KeypadDivide, "Keypad /"));
  AddKeyInputSlot(Key::kKeypadMultiply, InputSlotDetails(InputSlots::KeypadMultiply, "Keypad *"));
  AddKeyInputSlot(Key::kKeypadMinus, InputSlotDetails(InputSlots::KeypadMinus, "Keypad -"));
  AddKeyInputSlot(Key::kKeypadPlus, InputSlotDetails(InputSlots::KeypadPlus, "Keypad +"));
  AddKeyInputSlot(Key::kKeypadEnter, InputSlotDetails(InputSlots::KeypadEnter, "Keypad Enter"));
  AddKeyInputSlot(Key::kKeypadEquals, InputSlotDetails(InputSlots::KeypadEquals, "Keypad ="));
  AddKeyInputSlot(Key::kUpArrow, InputSlotDetails(InputSlots::UpArrow, "Up"));
  AddKeyInputSlot(Key::kDownArrow, InputSlotDetails(InputSlots::DownArrow, "Down"));
  AddKeyInputSlot(Key::kRightArrow, InputSlotDetails(InputSlots::RightArrow, "Right"));
  AddKeyInputSlot(Key::kLeftArrow, InputSlotDetails(InputSlots::LeftArrow, "Left"));
  AddKeyInputSlot(Key::kInsert, InputSlotDetails(InputSlots::Insert, "Insert"));
  AddKeyInputSlot(Key::kHome, InputSlotDetails(InputSlots::Home, "Home"));
  AddKeyInputSlot(Key::kEnd, InputSlotDetails(InputSlots::End, "End"));
  AddKeyInputSlot(Key::kPageUp, InputSlotDetails(InputSlots::PageUp, "Page Up"));
  AddKeyInputSlot(Key::kPageDown, InputSlotDetails(InputSlots::PageDown, "Page Down"));
  AddKeyInputSlot(Key::kF1, InputSlotDetails(InputSlots::F1, "F1"));
  AddKeyInputSlot(Key::kF2, InputSlotDetails(InputSlots::F2, "F2"));
  AddKeyInputSlot(Key::kF3, InputSlotDetails(InputSlots::F3, "F3"));
  AddKeyInputSlot(Key::kF4, InputSlotDetails(InputSlots::F4, "F4"));
  AddKeyInputSlot(Key::kF5, InputSlotDetails(InputSlots::F5, "F5"));
  AddKeyInputSlot(Key::kF6, InputSlotDetails(InputSlots::F6, "F6"));
  AddKeyInputSlot(Key::kF7, InputSlotDetails(InputSlots::F7, "F7"));
  AddKeyInputSlot(Key::kF8, InputSlotDetails(InputSlots::F8, "F8"));
  AddKeyInputSlot(Key::kF9, InputSlotDetails(InputSlots::F9, "F9"));
  AddKeyInputSlot(Key::kF10, InputSlotDetails(InputSlots::F10, "F10"));
  AddKeyInputSlot(Key::kF11, InputSlotDetails(InputSlots::F11, "F11"));
  AddKeyInputSlot(Key::kF12, InputSlotDetails(InputSlots::F12, "F12"));
  AddKeyInputSlot(Key::kF13, InputSlotDetails(InputSlots::F13, "F13"));
  AddKeyInputSlot(Key::kF14, InputSlotDetails(InputSlots::F14, "F14"));
  AddKeyInputSlot(Key::kF15, InputSlotDetails(InputSlots::F15, "F15"));
  AddKeyInputSlot(Key::kAlpha0, InputSlotDetails(InputSlots::Alpha0, "0"));
  AddKeyInputSlot(Key::kAlpha1, InputSlotDetails(InputSlots::Alpha1, "1"));
  AddKeyInputSlot(Key::kAlpha2, InputSlotDetails(InputSlots::Alpha2, "2"));
  AddKeyInputSlot(Key::kAlpha3, InputSlotDetails(InputSlots::Alpha3, "3"));
  AddKeyInputSlot(Key::kAlpha4, InputSlotDetails(InputSlots::Alpha4, "4"));
  AddKeyInputSlot(Key::kAlpha5, InputSlotDetails(InputSlots::Alpha5, "5"));
  AddKeyInputSlot(Key::kAlpha6, InputSlotDetails(InputSlots::Alpha6, "6"));
  AddKeyInputSlot(Key::kAlpha7, InputSlotDetails(InputSlots::Alpha7, "7"));
  AddKeyInputSlot(Key::kAlpha8, InputSlotDetails(InputSlots::Alpha8, "8"));
  AddKeyInputSlot(Key::kAlpha9, InputSlotDetails(InputSlots::Alpha9, "9"));
  AddKeyInputSlot(Key::kExclaim, InputSlotDetails(InputSlots::Exclaim, "!"));
  AddKeyInputSlot(Key::kDoubleQuote, InputSlotDetails(InputSlots::DoubleQuote, "\""));
  AddKeyInputSlot(Key::kHash, InputSlotDetails(InputSlots::Hash, "#"));
  AddKeyInputSlot(Key::kDollar, InputSlotDetails(InputSlots::Dollar, "$"));
  AddKeyInputSlot(Key::kPercent, InputSlotDetails(InputSlots::Percent, "%"));
  AddKeyInputSlot(Key::kAmpersand, InputSlotDetails(InputSlots::Ampersand, "&"));
  AddKeyInputSlot(Key::kQuote, InputSlotDetails(InputSlots::Quote, "'"));
  AddKeyInputSlot(Key::kLeftParen, InputSlotDetails(InputSlots::LeftParen, "("));
  AddKeyInputSlot(Key::kRightParen, InputSlotDetails(InputSlots::RightParen, ")"));
  AddKeyInputSlot(Key::kAsterisk, InputSlotDetails(InputSlots::Asterisk, "*"));
  AddKeyInputSlot(Key::kPlus, InputSlotDetails(InputSlots::Plus, "+"));
  AddKeyInputSlot(Key::kComma, InputSlotDetails(InputSlots::Comma, ","));
  AddKeyInputSlot(Key::kMinus, InputSlotDetails(InputSlots::Minus, "-"));
  AddKeyInputSlot(Key::kPeriod, InputSlotDetails(InputSlots::Period, "."));
  AddKeyInputSlot(Key::kSlash, InputSlotDetails(InputSlots::Slash, "/"));
  AddKeyInputSlot(Key::kColon, InputSlotDetails(InputSlots::Colon, ":"));
  AddKeyInputSlot(Key::kSemicolon, InputSlotDetails(InputSlots::Semicolon, ";"));
  AddKeyInputSlot(Key::kLess, InputSlotDetails(InputSlots::Less, "<"));
  AddKeyInputSlot(Key::kEquals, InputSlotDetails(InputSlots::Equals, "="));
  AddKeyInputSlot(Key::kGreater, InputSlotDetails(InputSlots::Greater, ">"));
  AddKeyInputSlot(Key::kQuestion, InputSlotDetails(InputSlots::Question, "?"));
  AddKeyInputSlot(Key::kAt, InputSlotDetails(InputSlots::At, "@"));
  AddKeyInputSlot(Key::kLeftBracket, InputSlotDetails(InputSlots::LeftBracket, "["));
  AddKeyInputSlot(Key::kBackslash, InputSlotDetails(InputSlots::Backslash, "\\"));
  AddKeyInputSlot(Key::kRightBracket, InputSlotDetails(InputSlots::RightBracket, "]"));
  AddKeyInputSlot(Key::kCaret, InputSlotDetails(InputSlots::Caret, "^"));
  AddKeyInputSlot(Key::kUnderscore, InputSlotDetails(InputSlots::Underscore, "_"));
  AddKeyInputSlot(Key::kBackQuote, InputSlotDetails(InputSlots::BackQuote, "`"));
  AddKeyInputSlot(Key::kA, InputSlotDetails(InputSlots::A, "A"));
  AddKeyInputSlot(Key::kB, InputSlotDetails(InputSlots::B, "B"));
  AddKeyInputSlot(Key::kC, InputSlotDetails(InputSlots::C, "C"));
  AddKeyInputSlot(Key::kD, InputSlotDetails(InputSlots::D, "D"));
  AddKeyInputSlot(Key::kE, InputSlotDetails(InputSlots::E, "E"));
  AddKeyInputSlot(Key::kF, InputSlotDetails(InputSlots::F, "F"));
  AddKeyInputSlot(Key::kG, InputSlotDetails(InputSlots::G, "G"));
  AddKeyInputSlot(Key::kH, InputSlotDetails(InputSlots::H, "H"));
  AddKeyInputSlot(Key::kI, InputSlotDetails(InputSlots::I, "I"));
  AddKeyInputSlot(Key::kJ, InputSlotDetails(InputSlots::J, "J"));
  AddKeyInputSlot(Key::kK, InputSlotDetails(InputSlots::K, "K"));
  AddKeyInputSlot(Key::kL, InputSlotDetails(InputSlots::L, "L"));
  AddKeyInputSlot(Key::kM, InputSlotDetails(InputSlots::M, "M"));
  AddKeyInputSlot(Key::kN, InputSlotDetails(InputSlots::N, "N"));
  AddKeyInputSlot(Key::kO, InputSlotDetails(InputSlots::O, "O"));
  AddKeyInputSlot(Key::kP, InputSlotDetails(InputSlots::P, "P"));
  AddKeyInputSlot(Key::kQ, InputSlotDetails(InputSlots::Q, "Q"));
  AddKeyInputSlot(Key::kR, InputSlotDetails(InputSlots::R, "R"));
  AddKeyInputSlot(Key::kS, InputSlotDetails(InputSlots::S, "S"));
  AddKeyInputSlot(Key::kT, InputSlotDetails(InputSlots::T, "T"));
  AddKeyInputSlot(Key::kU, InputSlotDetails(InputSlots::U, "U"));
  AddKeyInputSlot(Key::kV, InputSlotDetails(InputSlots::V, "V"));
  AddKeyInputSlot(Key::kW, InputSlotDetails(InputSlots::W, "W"));
  AddKeyInputSlot(Key::kX, InputSlotDetails(InputSlots::X, "X"));
  AddKeyInputSlot(Key::kY, InputSlotDetails(InputSlots::Y, "Y"));
  AddKeyInputSlot(Key::kZ, InputSlotDetails(InputSlots::Z, "Z"));
  AddKeyInputSlot(Key::kNumLock, InputSlotDetails(InputSlots::NumLock, "Num Lock"));
  AddKeyInputSlot(Key::kCapsLock, InputSlotDetails(InputSlots::CapsLock, "Caps Lock"));
  AddKeyInputSlot(Key::kScrollLock, InputSlotDetails(InputSlots::ScrollLock, "Scroll Lock"));

  AddKeyInputSlot(Key::kRightShift, InputSlotDetails(InputSlots::RightShift, "Right Shift", InputSlotDetails::Flags::kModifierKey));
  AddKeyInputSlot(Key::kLeftShift, InputSlotDetails(InputSlots::LeftShift, "Left Shift", InputSlotDetails::Flags::kModifierKey));
  AddKeyInputSlot(Key::kRightControl, InputSlotDetails(InputSlots::RightControl, "Right Ctrl", InputSlotDetails::Flags::kModifierKey));
  AddKeyInputSlot(Key::kLeftControl, InputSlotDetails(InputSlots::LeftControl, "Left Ctrl", InputSlotDetails::Flags::kModifierKey));
  AddKeyInputSlot(Key::kRightAlt, InputSlotDetails(InputSlots::RightAlt, "Right Alt", InputSlotDetails::Flags::kModifierKey));
  AddKeyInputSlot(Key::kLeftAlt, InputSlotDetails(InputSlots::LeftAlt, "Left Alt", InputSlotDetails::Flags::kModifierKey));
  AddKeyInputSlot(Key::kLeftMeta, InputSlotDetails(InputSlots::LeftMeta, "Left Meta", InputSlotDetails::Flags::kModifierKey));
  AddKeyInputSlot(Key::kRightMeta, InputSlotDetails(InputSlots::RightMeta, "Right Meta", InputSlotDetails::Flags::kModifierKey));

  AddKeyInputSlot(Key::kHelp, InputSlotDetails(InputSlots::Help, "Help"));
  AddKeyInputSlot(Key::kPrint, InputSlotDetails(InputSlots::Print, "Print Screen"));
  AddKeyInputSlot(Key::kSysReq, InputSlotDetails(InputSlots::SysReq, "Sys Req"));
  AddKeyInputSlot(Key::kMenu, InputSlotDetails(InputSlots::Menu, "Menu"));
  // clang-format on
}

void InputSlots::GetAllInputSlots(std::vector<InputSlot> &out_keys) {
  out_keys.clear();
  out_keys.reserve(slots_.size());
  for (const auto &key : slots_ | std::views::keys) {
    out_keys.push_back(key);
  }
}

auto InputSlots::GetInputSlotForKey(const Key key) -> InputSlot {
  const auto found = key_slots_.find(key);
  // We normally have a slot for every value defined in the Key enum
  if (found == key_slots_.cend()) {
    ASLOG_TO_LOGGER(platform_logger, critical,
        "We normally have a slot for every value defined in the Key enum, but "
        "key: {} does not have a corresponding slot.",
        static_cast<std::underlying_type_t<Key>>(key));
  }
  return found->second;
}

auto InputSlots::GetInputSlotDetails(
    const InputSlot &slot) -> std::shared_ptr<InputSlotDetails> {
  const auto found = slots_.find(slot);
  return (found != slots_.end()) ? found->second
                                 : std::shared_ptr<InputSlotDetails>{};
}

auto InputSlots::GetCategoryDisplayName(
    const std::string_view category_name) -> std::string_view {
  const auto found = categories_.find(category_name);
  return (found != categories_.cend()) ? found->second.display_string
                                       : std::string_view{"UNKNOWN_CATEGORY"};
}