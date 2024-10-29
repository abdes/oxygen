//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <string>

namespace oxygen::version {

auto Major() -> std::uint8_t;
auto Minor() -> std::uint8_t;
auto Patch() -> std::uint8_t;

auto Version() -> std::string;
auto VersionFull() -> std::string;
auto NameVersion() -> std::string;

} // namespace oxygen::version
