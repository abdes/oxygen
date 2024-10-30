//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include "oxygen/base/resource.h"

#include <utility>

#include "oxygen/base/resource_handle.h"
#include "oxygen/world/types.h"

namespace oxygen::world {

namespace entity {
struct Descriptor {
  TransformDescriptor *transform;
};

auto CreateGameEntity(const EntityDescriptor &entity_desc) -> Entity;
auto RemoveGameEntity(const Entity &entity) -> size_t;

} // namespace entity

// Transforms are only created and removed with entities, and all game
// entities must have a transform component. Therefore, it will always be
// true that for a game entity resource handle, the index and generation
// will be the same as for the corresponding transform.

class Entity : public Resource<resources::kEntity> {
public:
  constexpr explicit Entity(const ResourceHandle &handle) : Resource(handle) {
  }
  constexpr Entity() = default;

  [[nodiscard]] constexpr auto GetEntityId() const noexcept -> EntityId {
    return GetId();
  }

  [[nodiscard]] constexpr auto GetTransformId() const noexcept -> TransformId {
    ResourceHandle transform_id(GetEntityId());
    transform_id.SetResourceType(resources::kTransform);
    return transform_id;
  }

  [[nodiscard]] auto GetTransform() const noexcept -> Transform;
};

} // namespace oxygen::world
