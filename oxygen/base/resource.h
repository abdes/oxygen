//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include "oxygen/base/resource_handle.h"
#include "oxygen/world/types.h"

namespace oxygen {

/**
 * A graphics API agnostic POD structure representing different types of
 * resources that get linked to their counterparts on the core backend.
 */
template <ResourceHandle::ResourceTypeT ResourceType> class Resource {
public:
  constexpr explicit Resource(ResourceHandle handle)
      : handle_(std::move(handle)) {
    assert(handle_.ResourceType() == ResourceType);
  }
  constexpr Resource() = default;

  [[nodiscard]] constexpr auto GetId() const noexcept
      -> const ResourceHandle & {
    return handle_;
  }

  [[nodiscard]] virtual constexpr auto IsValid() const noexcept -> bool {
    return handle_.IsValid();
  }

private:
  ResourceHandle handle_;
};

} // namespace oxygen
