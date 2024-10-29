//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"

#include "oxygen/engine-dll/api.h"

// NOLINTNEXTLINE
TEST(LinkedEngineDllTest, CanUseApi) {
  CreateGameEntity();
}
