//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "oxygen/base/types.h"
#include "oxygen/platform/types.h"

#include "types.h"

namespace oxygen::input {

class InputMappingContext {
public:
  explicit InputMappingContext(std::string name);

  void AddMapping(std::shared_ptr<InputActionMapping> mapping);

  [[nodiscard]] auto GetName() const {
    return name_;
  }

  void HandleInput(
      const platform::InputSlot &slot, const platform::InputEvent &event) const;

  [[nodiscard]] bool Update(Duration delta_time) const;

private:
  std::string name_;

  std::vector<std::shared_ptr<InputActionMapping>> mappings_;
};

} // namespace oxygen::input
