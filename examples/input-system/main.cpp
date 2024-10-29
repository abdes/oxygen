//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "main_module.h"

#include <chrono>
#include <memory>
#include <span>

#include "oxygen/core/engine.h"
#include "oxygen/core/version.h"
#include "oxygen/logging/logging.h"
#include "oxygen/platform-sdl/platform.h"

using namespace std::chrono_literals;

using oxygen::Engine;

auto main(
    [[maybe_unused]] const int argc, [[maybe_unused]] char *argv[]) -> int {
  auto status{EXIT_SUCCESS};

  auto &logger = oxygen::log::Registry::Instance().GetLogger("main");
  ASLOG_TO_LOGGER(logger, info, "{}", oxygen::version::NameVersion());

  // We want to control the destruction order of the important objects in the
  // system. For example, destroy the core before we destroy the platform.
  std::shared_ptr<oxygen::Platform> platform;
  std::shared_ptr<oxygen::Engine> engine;

  try {
    platform = std::make_shared<oxygen::platform::sdl::Platform>();

    Engine::Properties props{
        .application =
            {
                .name = "Triangle",
                .version = 0x0001'0000,
            },
        .extensions = {},
        .max_fixed_update_duration = 10ms,
    };

    engine = std::make_shared<Engine>(*platform, props);

    auto my_module = std::make_shared<MainModule>(*engine);
    engine->AddModule(my_module);

    engine->Run();

    ASLOG_TO_LOGGER(logger, info, "Exiting application");
  } catch (std::exception const &err) {
    ASLOG_TO_LOGGER(logger, error, "A fatal error occurred: {}", err.what());
    status = EXIT_FAILURE;
  }

  // Explicit destruction order due to dependencies.
  engine.reset();
  platform.reset();

  return status;
}
