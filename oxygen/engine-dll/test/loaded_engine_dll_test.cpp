//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include <windows.h>

#include "gtest/gtest.h"

#include "oxygen/engine-dll/api.h"

// NOLINTNEXTLINE
TEST(LoadedEngineDllTest, CanUseApi) {
  typedef ResourceHandle::HandleT (*CreateGameEntityFunc)();

  // Load the DLL
  HMODULE hModule = LoadLibrary(TEXT("engine-dll.dll"));
  ASSERT_NE(hModule, nullptr);

  // Get the function pointer
  CreateGameEntityFunc CreateGameEntity =
      (CreateGameEntityFunc)GetProcAddress(hModule, "CreateGameEntity");
  ASSERT_NE(CreateGameEntity, nullptr);

  // Call the function
  CreateGameEntity();

  // Free the DLL
  FreeLibrary(hModule);
}
