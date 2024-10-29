//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "detail/base_window.h"

#include "oxygen/logging/logging.h"
#include "oxygen/platform/window.h"

namespace {
auto &platform_logger = // NOLINT(*-avoid-non-const-global-variables)
    oxygen::log::Registry::Instance().GetLogger("Oxygen.Platform");
}

using oxygen::platform::Window;

namespace {

// auto &platform_logger =
// asap::logging::Registry::GetLogger("Oxygen.Platform");

auto CheckNotInFullScreenMode(
    const Window &window, const std::string &operation) -> bool {
  if (window.IsFullScreen()) {
    ASLOG_TO_LOGGER(platform_logger, warn,
        "Window [{}] is in full-screen mode and cannot be {}d. Call "
        "`FullScreen(off)` first.",
        window.Id(), operation);
    return false;
  }
  return true;
}

auto CheckNotBorderless(
    const Window &window, const std::string &operation) -> bool {
  if (window.IsBorderLess()) {
    ASLOG_TO_LOGGER(platform_logger, warn,
        "Window [{}] is borderless and cannot be {}d.", window.Id(), operation);
    return false;
  }
  return true;
}

auto CheckNotMinimized(
    const Window &window, const std::string &operation) -> bool {
  if (window.IsMinimized()) {
    ASLOG_TO_LOGGER(platform_logger, warn,
        "Window [{}] is minimized and cannot be {}d. Call `Restore()` first.",
        window.Id(), operation);
    return false;
  }
  return true;
}

auto CheckIsResizable(
    const Window &window, const std::string &operation) -> bool {
  if (!window.IsResizable()) {
    ASLOG_TO_LOGGER(platform_logger, warn,
        "Window [{}] is setup to be not resizable and cannot be {}d.",
        window.Id(), operation);
    return false;
  }
  return true;
}
} // namespace

Window::Window() : pimpl_(new BaseWindow()) {
}

Window::~Window() {
  delete pimpl_;
}

void Window::Maximize() {
  if (CheckNotInFullScreenMode(*this, "maximize") &&
      CheckNotMinimized(*this, "maximize")) {
    DoMaximize();
  }
}

void Window::Restore() {
  if (CheckNotInFullScreenMode(*this, "restore")) {
    DoRestore();
  }
}

void Window::Size(const PixelExtent &extent) {
  if (CheckNotInFullScreenMode(*this, "resize") &&
      CheckNotBorderless(*this, "resize") &&
      CheckIsResizable(*this, "resize") && CheckNotMinimized(*this, "resize")) {
    DoResize(extent);
  }
}

void Window::Position(const PixelPosition &position) {
  if (CheckNotInFullScreenMode(*this, "re-position") &&
      CheckNotMinimized(*this, "resize")) {
    if (IsMaximized()) {
      DoRestore();
    }
    DoPosition(position);
  }
}

void Window::RequestNotToClose() const {
  pimpl_->RequestNotToClose();
}

void Window::RequestClose(const bool force) {
  if (ShouldClose()) {
    ASLOG_TO_LOGGER(platform_logger, info,
        "Ongoing request to close the window exists, ignoring new request");
    return;
  }
  ASLOG_TO_LOGGER(
      platform_logger, info, "Window [id = {}] is requested to close", Id());
  pimpl_->RequestClose(force);
  if (ShouldClose()) {
    ProcessCloseRequest(force);
    // Window will be closed and destroyed
  } else {
    pimpl_->CancelCloseRequest();
  }
}

auto Window::ShouldClose() const -> bool {
  return pimpl_->ShouldClose();
}

auto Window::OnCloseRequested() const -> sigslot::signal<bool> & {
  return pimpl_->OnCloseRequested();
}

auto Window::OnResized() const -> sigslot::signal<oxygen::PixelExtent> & {
  return pimpl_->OnResized();
}

auto Window::OnMinimized() const -> sigslot::signal<> & {
  return pimpl_->OnMinimized();
}

auto Window::OnMaximized() const -> sigslot::signal<> & {
  return pimpl_->OnMaximized();
}

auto Window::OnRestored() const -> sigslot::signal<> & {
  return pimpl_->OnRestored();
}
