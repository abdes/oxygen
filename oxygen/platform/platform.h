//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "sigslot/signal.hpp"

#include "oxygen/base/macros.h"
#include "oxygen/platform/display.h"
#include "oxygen/platform/input_event.h"
#include "oxygen/platform/types.h"
#include "oxygen/platform/window.h"

namespace oxygen {

class Platform {
public:
  Platform();
  virtual ~Platform() = default;

  OXYGEN_MAKE_NON_COPYABLE(Platform)
  OXYGEN_MAKE_NON_MOVEABLE(Platform)

  // ---------------------------------------------------------------------------

  [[nodiscard]] virtual auto
  GetRequiredInstanceExtensions() const -> std::vector<const char *> = 0;

  // -- Displays ---------------------------------------------------------------

  [[nodiscard]] virtual auto
  Displays() const -> std::vector<std::unique_ptr<platform::Display>> = 0;

  [[nodiscard]] virtual auto DisplayFromId(
      const platform::Display::IdType &display_id) const
      -> std::unique_ptr<platform::Display> = 0;

  // -- Window Management ------------------------------------------------------

  virtual auto MakeWindow(std::string const &title,
      PixelExtent const &extent) -> std::weak_ptr<platform::Window> = 0;

  virtual auto MakeWindow(std::string const &title, PixelExtent const &extent,
      platform::Window::InitialFlags flags)
      -> std::weak_ptr<platform::Window> = 0;

  virtual auto MakeWindow(std::string const &title,
      PixelPosition const &position,
      PixelExtent const &extent) -> std::weak_ptr<platform::Window> = 0;

  virtual auto MakeWindow(std::string const &title,
      PixelPosition const &position, PixelExtent const &extent,
      platform::Window::InitialFlags flags)
      -> std::weak_ptr<platform::Window> = 0;

  // -- Events -----------------------------------------------------------------

  virtual auto PollEvent() -> std::unique_ptr<platform::InputEvent> = 0;

  // -- Slots ------------------------------------------------------------------

  [[nodiscard]] auto OnLastWindowClosed() -> auto & {
    return on_last_window_closed_;
  }

  [[nodiscard]] auto OnWindowClosed() -> auto & {
    return on_window_closed_;
  }

  [[nodiscard]] auto OnDisplayConnected() -> auto & {
    return on_display_connected_;
  }

  [[nodiscard]] auto OnDisplayDisconnected() -> auto & {
    return on_display_disconnected_;
  }

  // To get the new orientation, find the display from its id and query its
  // orientation.
  [[nodiscard]] auto OnDisplayOrientationChanged() -> auto & {
    return on_display_orientation_changed_;
  }

  static void GetAllInputSlots(std::vector<platform::InputSlot> &out_keys);
  static auto GetInputSlotForKey(platform::Key key) -> platform::InputSlot;

  auto GetInputCategoryDisplayName(
      std::string_view category_name) -> std::string_view;

private:
  sigslot::signal<> on_last_window_closed_;
  sigslot::signal<platform::Window const &> on_window_closed_;

  sigslot::signal<platform::Display::IdType> on_display_connected_;
  sigslot::signal<platform::Display::IdType> on_display_disconnected_;
  sigslot::signal<platform::Display::IdType> on_display_orientation_changed_;
};

} // namespace oxygen
