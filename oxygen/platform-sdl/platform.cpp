//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "platform.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <memory>
#include <span>

#include <oxygen/base/compilers.h>
// Disable compiler and linter warnings originating from 'fmt' and for which we
// cannot do anything.
OXYGEN_DIAGNOSTIC_PUSH
#if defined(__clang__) || defined(ASAP_GNUC_VERSION)
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#endif
#include "fmt/core.h"
OXYGEN_DIAGNOSTIC_POP

#include "oxygen/logging/logging.h"

namespace {
auto &platform_logger = // NOLINT(*-avoid-non-const-global-variables)
    oxygen::log::Registry::Instance().GetLogger("Oxygen.Platform");
}

#include "oxygen/base/types.h"
#include "oxygen/platform/input_event.h"
#include "oxygen/platform/types.h"
#include "oxygen/platform/window.h"

#include "detail/wrapper.h"
#include "display.h"
#include "window.h"

using oxygen::Duration;
using oxygen::SubPixelMotion;
using oxygen::SubPixelPosition;
using oxygen::platform::ButtonState;
using oxygen::platform::InputEvent;
using oxygen::platform::Key;
using oxygen::platform::KeyEvent;
using oxygen::platform::MouseButton;
using oxygen::platform::MouseButtonEvent;
using oxygen::platform::MouseMotionEvent;
using oxygen::platform::MouseWheelEvent;
using oxygen::platform::sdl::Platform;

namespace {
auto MapKeyCode(SDL_Keycode code) {
  using oxygen::platform::Key;
  // clang-format off
  switch (code) {
    case SDLK_BACKSPACE: return Key::kBackSpace;
    case SDLK_DELETE: return Key::kDelete;
    case SDLK_TAB: return Key::kTab;
    case SDLK_CLEAR: return Key::kClear;
    case SDLK_RETURN: return Key::kReturn;
    case SDLK_PAUSE: return Key::kPause;
    case SDLK_ESCAPE: return Key::kEscape;
    case SDLK_SPACE: return Key::kSpace;
    case SDLK_KP_0: return Key::kKeypad0;
    case SDLK_KP_1: return Key::kKeypad1;
    case SDLK_KP_2: return Key::kKeypad2;
    case SDLK_KP_3: return Key::kKeypad3;
    case SDLK_KP_4: return Key::kKeypad4;
    case SDLK_KP_5: return Key::kKeypad5;
    case SDLK_KP_6: return Key::kKeypad6;
    case SDLK_KP_7: return Key::kKeypad7;
    case SDLK_KP_8: return Key::kKeypad8;
    case SDLK_KP_9: return Key::kKeypad9;
    case SDLK_KP_PERIOD: return Key::kKeypadPeriod;
    case SDLK_KP_DIVIDE: return Key::kKeypadDivide;
    case SDLK_KP_MULTIPLY: return Key::kKeypadMultiply;
    case SDLK_KP_MINUS: return Key::kKeypadMinus;
    case SDLK_KP_PLUS: return Key::kKeypadPlus;
    case SDLK_KP_ENTER: return Key::kKeypadEnter;
    case SDLK_KP_EQUALS: return Key::kKeypadEquals;
    case SDLK_UP: return Key::kUpArrow;
    case SDLK_DOWN: return Key::kDownArrow;
    case SDLK_RIGHT: return Key::kRightArrow;
    case SDLK_LEFT: return Key::kLeftArrow;
    case SDLK_INSERT: return Key::kInsert;
    case SDLK_HOME: return Key::kHome;
    case SDLK_END: return Key::kEnd;
    case SDLK_PAGEUP: return Key::kPageUp;
    case SDLK_PAGEDOWN: return Key::kPageDown;
    case SDLK_F1: return Key::kF1;
    case SDLK_F2: return Key::kF2;
    case SDLK_F3: return Key::kF3;
    case SDLK_F4: return Key::kF4;
    case SDLK_F5: return Key::kF5;
    case SDLK_F6: return Key::kF6;
    case SDLK_F7: return Key::kF7;
    case SDLK_F8: return Key::kF8;
    case SDLK_F9: return Key::kF9;
    case SDLK_F10: return Key::kF10;
    case SDLK_F11: return Key::kF11;
    case SDLK_F12: return Key::kF12;
    case SDLK_F13: return Key::kF13;
    case SDLK_F14: return Key::kF14;
    case SDLK_F15: return Key::kF15;
    case SDLK_0: return Key::kAlpha0;
    case SDLK_1: return Key::kAlpha1;
    case SDLK_2: return Key::kAlpha2;
    case SDLK_3: return Key::kAlpha3;
    case SDLK_4: return Key::kAlpha4;
    case SDLK_5: return Key::kAlpha5;
    case SDLK_6: return Key::kAlpha6;
    case SDLK_7: return Key::kAlpha7;
    case SDLK_8: return Key::kAlpha8;
    case SDLK_9: return Key::kAlpha9;
    case SDLK_EXCLAIM: return Key::kExclaim;
    case SDLK_DBLAPOSTROPHE: return Key::kDoubleQuote;
    case SDLK_HASH: return Key::kHash;
    case SDLK_DOLLAR: return Key::kDollar;
    case SDLK_PERCENT: return Key::kPercent;
    case SDLK_AMPERSAND: return Key::kAmpersand;
    case SDLK_APOSTROPHE: return Key::kQuote;
    case SDLK_LEFTPAREN: return Key::kLeftParen;
    case SDLK_RIGHTPAREN: return Key::kRightParen;
    case SDLK_ASTERISK: return Key::kAsterisk;
    case SDLK_PLUS: return Key::kPlus;
    case SDLK_COMMA: return Key::kComma;
    case SDLK_MINUS: return Key::kMinus;
    case SDLK_PERIOD: return Key::kPeriod;
    case SDLK_SLASH: return Key::kSlash;
    case SDLK_COLON: return Key::kColon;
    case SDLK_SEMICOLON: return Key::kSemicolon;
    case SDLK_LESS: return Key::kLess;
    case SDLK_EQUALS: return Key::kEquals;
    case SDLK_GREATER: return Key::kGreater;
    case SDLK_QUESTION: return Key::kQuestion;
    case SDLK_AT: return Key::kAt;
    case SDLK_LEFTBRACKET: return Key::kLeftBracket;
    case SDLK_BACKSLASH: return Key::kBackslash;
    case SDLK_RIGHTBRACKET: return Key::kRightBracket;
    case SDLK_CARET: return Key::kCaret;
    case SDLK_UNDERSCORE: return Key::kUnderscore;
    case SDLK_GRAVE: return Key::kBackQuote;
    case SDLK_A: return Key::kA;
    case SDLK_B: return Key::kB;
    case SDLK_C: return Key::kC;
    case SDLK_D: return Key::kD;
    case SDLK_E: return Key::kE;
    case SDLK_F: return Key::kF;
    case SDLK_G: return Key::kG;
    case SDLK_H: return Key::kH;
    case SDLK_I: return Key::kI;
    case SDLK_J: return Key::kJ;
    case SDLK_K: return Key::kK;
    case SDLK_L: return Key::kL;
    case SDLK_M: return Key::kM;
    case SDLK_N: return Key::kN;
    case SDLK_O: return Key::kO;
    case SDLK_P: return Key::kP;
    case SDLK_Q: return Key::kQ;
    case SDLK_R: return Key::kR;
    case SDLK_S: return Key::kS;
    case SDLK_T: return Key::kT;
    case SDLK_U: return Key::kU;
    case SDLK_V: return Key::kV;
    case SDLK_W: return Key::kW;
    case SDLK_X: return Key::kX;
    case SDLK_Y: return Key::kY;
    case SDLK_Z: return Key::kZ;
    case SDLK_NUMLOCKCLEAR: return Key::kNumLock;
    case SDLK_CAPSLOCK: return Key::kCapsLock;
    case SDLK_SCROLLLOCK: return Key::kScrollLock;
    case SDLK_RSHIFT: return Key::kRightShift;
    case SDLK_LSHIFT: return Key::kLeftShift;
    case SDLK_RCTRL: return Key::kRightControl;
    case SDLK_LCTRL: return Key::kLeftControl;
    case SDLK_RALT: return Key::kRightAlt;
    case SDLK_LALT: return Key::kLeftAlt;
    case SDLK_LGUI: return Key::kLeftMeta;
    case SDLK_RGUI: return Key::kRightMeta;
    case SDLK_HELP: return Key::kHelp;
    case SDLK_PRINTSCREEN: return Key::kPrint;
    case SDLK_SYSREQ: return Key::kSysReq;
    case SDLK_MENU: return Key::kMenu;
    default: return Key::kNone;
  }
  // clang-format on
}

auto TranslateKeyboardEvent(
    SDL_Event const &event) -> std::unique_ptr<InputEvent> {
  const auto key_code = MapKeyCode(event.key.key);
  if (key_code == Key::kNone) {
    // This is not a key code we are interested to handle.
    // Do not generate an event for it
    uint32_t key = event.key.key;
    uint32_t scan_code = event.key.scancode;
    ASDEBUG_TO_LOGGER(platform_logger,
        "Keyboard event with key code = {} (scan code = {}) is not "
        "something we can handle. Ignoring event.", key, scan_code);
    return {};
  }

  const KeyEvent::KeyInfo key_info(key_code, event.key.repeat);
  const ButtonState button_state =
      event.key.down ? ButtonState::kPressed : ButtonState::kReleased;

  auto key_event = std::make_unique<KeyEvent>(&event,
      std::chrono::duration_cast<oxygen::TimePoint>(
          std::chrono::nanoseconds(event.key.timestamp)),
      key_info, button_state);
  key_event->SetWindowId(event.key.windowID);

  return key_event;
}

auto MapMouseButton(auto button) {
  using oxygen::platform::MouseButton;
  // clang-format off
  switch (button) {
    case SDL_BUTTON_LEFT: return MouseButton::kLeft;
    case SDL_BUTTON_RIGHT: return MouseButton::kRight;
    case SDL_BUTTON_MIDDLE: return MouseButton::kMiddle;
    case SDL_BUTTON_X1: return MouseButton::kExtButton1;
    case SDL_BUTTON_X2: return MouseButton::kExtButton2;
    default: return MouseButton::kNone;
  }
  // clang-format on
}

auto TranslateMouseButtonEvent(
    const SDL_Event &event) -> std::unique_ptr<InputEvent> {
  const auto button = MapMouseButton(event.button.button);
  if (button == MouseButton::kNone) {
    // This is not a mouse button we are interested to handle.
    // Do not generate an event for it
    ASDEBUG_TO_LOGGER(platform_logger,
        "Mouse button event with button = {} is not something we can handle. "
        "Ignoring event.",
        event.button.button);
    return {};
  }

  const ButtonState button_state =
      event.button.down ? ButtonState::kPressed : ButtonState::kReleased;

  auto button_event = std::make_unique<MouseButtonEvent>(&event,
      std::chrono::duration_cast<oxygen::TimePoint>(
          std::chrono::nanoseconds(event.button.timestamp)),
      SubPixelPosition{
          .x = event.button.x,
          .y = event.button.y,
      },
      button, button_state);
  button_event->SetWindowId(event.key.windowID);
  return button_event;
}

auto TranslateMouseMotionEvent(
    const SDL_Event &event) -> std::unique_ptr<InputEvent> {
  auto motion_event = std::make_unique<MouseMotionEvent>(&event,
      std::chrono::duration_cast<oxygen::TimePoint>(
          std::chrono::nanoseconds(event.motion.timestamp)),
      SubPixelPosition{
          .x = event.motion.x,
          .y = event.motion.y,
      },
      SubPixelMotion{
          .dx = event.motion.xrel,
          .dy = event.motion.yrel,
      });
  motion_event->SetWindowId(event.key.windowID);
  return motion_event;
}

auto TranslateMouseWheelEvent(
    const SDL_Event &event) -> std::unique_ptr<InputEvent> {
  const auto direction =
      event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1.0F : -1.0F;

  auto wheel_event = std::make_unique<MouseWheelEvent>(&event,
      std::chrono::duration_cast<oxygen::TimePoint>(
          std::chrono::nanoseconds(event.wheel.timestamp)),
      SubPixelPosition{
          .x = event.wheel.mouse_x,
          .y = event.wheel.mouse_y,
      },
      SubPixelMotion{
          .dx = direction * event.wheel.x,
          .dy = direction * event.wheel.y,
      });
  wheel_event->SetWindowId(event.key.windowID);
  return wheel_event;
}
} // namespace

Platform::Platform(std::shared_ptr<detail::WrapperInterface> sdl_wrapper)
    : sdl_(sdl_wrapper ? std::move(sdl_wrapper)
                       : std::make_shared<detail::Wrapper>()) {
  sdl_->Init(SDL_INIT_VIDEO);
  sdl_->SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "0");
  ASLOG_TO_LOGGER(platform_logger, info, "Platform/SDL3 initialized");
}

Platform::~Platform() {
  // last attempt to clean up before calling SDL to shut down.
  // Normally all windows should have been properly closed by the application
  // module that created them.
  windows_.clear();

  ASLOG_TO_LOGGER(platform_logger, info, "Platform/SDL3 destroyed");

  // ->Final<- thing to do is to terminate SDL3.
  sdl_->Terminate();
}

auto Platform::GetRequiredInstanceExtensions() const
    -> std::vector<const char *> {
  return sdl_->GetRequiredVulkanExtensions();
}

auto Platform::MakeWindow(std::string const &title,
    PixelExtent const &extent) -> std::weak_ptr<platform::Window> {
  auto new_window = std::make_shared<Window>(title, extent);
  windows_.push_back(new_window);
  return new_window;
}

auto Platform::MakeWindow(std::string const &title, PixelExtent const &extent,
    platform::Window::InitialFlags flags) -> std::weak_ptr<platform::Window> {
  auto new_window = std::make_shared<Window>(title, extent, flags);
  windows_.push_back(new_window);
  return new_window;
}

auto Platform::MakeWindow(std::string const &title,
    PixelPosition const &position,
    PixelExtent const &extent) -> std::weak_ptr<platform::Window> {
  auto new_window = std::make_shared<Window>(title, position, extent);
  windows_.push_back(new_window);
  return new_window;
}

auto Platform::MakeWindow(std::string const &title,
    PixelPosition const &position, PixelExtent const &extent,
    platform::Window::InitialFlags flags) -> std::weak_ptr<platform::Window> {
  auto new_window = std::make_shared<Window>(title, position, extent, flags);
  windows_.push_back(new_window);
  return new_window;
}

auto Platform::Displays() const
    -> std::vector<std::unique_ptr<platform::Display>> {
  std::vector<std::unique_ptr<platform::Display>> displays;

  int display_count{0};
  SDL_DisplayID *display_ids = sdl_->GetDisplays(&display_count);

  for (const std::span s_display_ids(
           display_ids, static_cast<std::size_t>(display_count));
       auto const &display_id : s_display_ids) {
    displays.emplace_back(std::make_unique<Display>(display_id));
  }

  // Free the memory allocated for the display_ids returned by SDL
  sdl_->Free(display_ids);

  return displays;
}

auto Platform::DisplayFromId(const platform::Display::IdType &display_id) const
    -> std::unique_ptr<platform::Display> {
  std::unique_ptr<platform::Display> display{};
  int display_count{0};
  try {
    SDL_DisplayID *display_ids = sdl_->GetDisplays(&display_count);

    for (const std::span s_display_ids(
             display_ids, static_cast<std::size_t>(display_count));
         auto const &a_display_id : s_display_ids) {
      if (a_display_id == display_id) {
        display = std::make_unique<Display>(display_id);
      }
    }
  } catch (...) { // NOLINT(*-empty-catch)
    // If an error occurs or there are no connected displays, we simply
    // return a nullptr value
  }
  return display;
}

auto Platform::PollEvent() -> std::unique_ptr<platform::InputEvent> {
  if (sdl_->PollEvent(&event_)) {
    if (event_.type == SDL_EVENT_KEY_UP || event_.type == SDL_EVENT_KEY_DOWN) {
      ASDEBUG_TO_LOGGER(platform_logger,
          "Keyboard event type = {} window id = {} repeat = {} keysim.scancode "
          "= {} keysim.keycode = {} key name = {}",
          ((event_.key.type == SDL_EVENT_KEY_UP) ? "KEY_UP" : "KEY_DOWN"),
          event_.key.windowID, event_.key.repeat, (uint32_t)event_.key.scancode,
          (uint32_t)event_.key.key, sdl_->GetKeyName(event_.key.key));
      return TranslateKeyboardEvent(event_);
    }
    if (event_.type == SDL_EVENT_MOUSE_BUTTON_UP ||
        event_.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      ASDEBUG_TO_LOGGER(platform_logger,
          "Mouse button event button = {} state = {}", event_.button.button,
          ((event_.button.type == SDL_EVENT_MOUSE_BUTTON_UP) ? "UP" : "DOWN"));
      return TranslateMouseButtonEvent(event_);
    }
    if (event_.type == SDL_EVENT_MOUSE_WHEEL) {
      ASDEBUG_TO_LOGGER(platform_logger, "Mouse wheel event dx = {} dy = {}",
          event_.wheel.x, event_.wheel.y);
      return TranslateMouseWheelEvent(event_);
    }
    if (event_.type == SDL_EVENT_MOUSE_MOTION) {
      ASDEBUG_TO_LOGGER(platform_logger, "Mouse motion event dx = {} dy = {}",
          event_.motion.xrel, event_.motion.yrel);
      return TranslateMouseMotionEvent(event_);
    }

    if (event_.type >= SDL_EVENT_DISPLAY_FIRST &&
        event_.type <= SDL_EVENT_DISPLAY_LAST) {
      DispatchDisplayEvent(event_);
    } else if (event_.type >= SDL_EVENT_WINDOW_FIRST &&
               event_.type <= SDL_EVENT_WINDOW_LAST) {
      DispatchWindowEvent(event_);
    } else if (event_.type == SDL_EVENT_POLL_SENTINEL) {
      // Signals the end of an event poll cycle
    } else {
      if (event_.type != SDL_EVENT_MOUSE_MOTION) {
        ASDEBUG_TO_LOGGER(platform_logger, "Event [{}] has no dispatcher",
            detail::SdlEventName(event_.type));
      }
      OnUnhandledEvent()(event_);
    }
  }
  return {};
}

auto Platform::WindowFromId(
    WindowIdType window_id) const -> platform::Window & {
  const auto found = std::ranges::find_if(windows_,
      [window_id](auto &window) { return window->Id() == window_id; });
  // We should only call this method when we are sure the window id is valid
  assert(found != windows_.end() && "We should only call this method when we "
                                    "are sure the window id is valid");
  return **found;
}

void Platform::DispatchDisplayEvent(SDL_Event const &event) {
  switch (event.type) {
  case SDL_EVENT_DISPLAY_ADDED:
    OnDisplayConnected()(event.display.displayID);
    break;

  case SDL_EVENT_DISPLAY_REMOVED:
    OnDisplayDisconnected()(event.display.displayID);
    break;

  case SDL_EVENT_DISPLAY_ORIENTATION:
    OnDisplayOrientationChanged()(event.display.displayID);
    break;

  case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
    [[fallthrough]];
  case SDL_EVENT_DISPLAY_MOVED:
    // As of now, we do not handle these events and, we do not have slots for
    // dispatching them.
    break;
  default:
    ASLOG_TO_LOGGER(platform_logger, warn,
        "Display event [{}] not expected by handler",
        detail::SdlEventName(event.type));
  }
}

void Platform::DispatchWindowEvent(SDL_Event const &event) {
  switch (event.type) {
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
    auto window_id = event.window.windowID;
    const auto the_window = std::ranges::find_if(windows_,
        [window_id](auto &window) { return window->Id() == window_id; });
    assert(the_window != windows_.end());
    ASLOG_TO_LOGGER(
        platform_logger, info, "Window [id = {}] is closing", window_id);
    OnWindowClosed()(**the_window);
    ASLOG_TO_LOGGER(
        platform_logger, info, "Window [id = {}] is closed", window_id);

    windows_.erase(the_window);

    if (windows_.empty()) {
      OnLastWindowClosed()();
    }
  } break;

  case SDL_EVENT_WINDOW_DESTROYED:
    assert(std::ranges::find_if(std::begin(windows_), std::end(windows_),
               [&event](auto const &window) {
                 return window->Id() == event.window.windowID;
               }) == std::end(windows_));
    ASLOG_TO_LOGGER(platform_logger, info,
        "Window [id = {}] was destroyed and is now no longer tracked",
        event.window.windowID);
    break;

  case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
    ASLOG_TO_LOGGER(platform_logger, info,
        "Window [id = {}] buffer size changed to {} x {}",
        event.window.windowID, event.window.data1, event.window.data2);

    // We don't rely on this unreliable event from SDL3. Instead, we handle
    // swapchain resizing and creation using vulkan view of the window
    // surface and the SDL_EVENT_WINDOW_RESIZED / SDL_EVENT_WINDOW_MINIMIZED /
    // SDL_EVENT_WINDOW_RESTORED.
  } break;

  case SDL_EVENT_WINDOW_RESIZED: {
    ASLOG_TO_LOGGER(platform_logger, info,
        "Window [id = {}] size changed to {} x {}", event.window.windowID,
        event.window.data1, event.window.data2);
    const auto &window = WindowFromId(event.window.windowID);
    window.OnResized()(
        PixelExtent{.width = event.window.data1, .height = event.window.data2});
  } break;

  case SDL_EVENT_WINDOW_MINIMIZED: {
    ASLOG_TO_LOGGER(platform_logger, info, "Window [id = {}] minimized",
        event.window.windowID);
    const auto &window = WindowFromId(event.window.windowID);
    window.OnMinimized()();
  } break;

  case SDL_EVENT_WINDOW_MAXIMIZED: {
    ASLOG_TO_LOGGER(platform_logger, info, "Window [id = {}] maximized",
        event.window.windowID);
    const auto &window = WindowFromId(event.window.windowID);
    window.OnMaximized()();
  } break;

  case SDL_EVENT_WINDOW_RESTORED: {
    ASLOG_TO_LOGGER(platform_logger, info, "Window [id = {}] restored",
        event.window.windowID);
    const auto &window = WindowFromId(event.window.windowID);
    window.OnRestored()();
  } break;

  default:
    ASLOG_TO_LOGGER(platform_logger, warn,
        "Window event [{}] not expected by handler",
        detail::SdlEventName(event.type));
  }
}
