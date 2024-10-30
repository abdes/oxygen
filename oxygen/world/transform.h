//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "oxygen/base/resource.h"
#include "oxygen/base/resource_handle.h"
#include "oxygen/world/types.h"

namespace oxygen::world {

namespace transform {
struct Descriptor {
  glm::vec3 position{};
  glm::quat rotation{};
  glm::vec3 scale{1.F, 1.F, 1.F};
};

auto CreateTransform(TransformDescriptor &transform_desc,
    const EntityId &entity_id) -> Transform;
auto RemoveTransform(const Transform &transform) -> size_t;
} // namespace transform

class Transform : public Resource<resources::kTransform> {
public:
  explicit Transform(const TransformId &transform_id) : Resource(transform_id) {
  }
  Transform() = default;

  [[nodiscard]] constexpr auto GetTransformId() const noexcept -> EntityId {
    return GetId();
  }

  [[nodiscard]] constexpr auto GetEntityId() const noexcept -> EntityId {
    // Transforms are only created and removed with entities, and all game
    // entities must have a transform component. Therefore, it will always be
    // true that for a game entity resource handle, the index and generation
    // will be the same as for the corresponding transform.
    ResourceHandle entity_id(GetTransformId());
    entity_id.SetResourceType(resources::kEntity);
    return entity_id;
  }

  [[nodiscard]] auto IsValid() const noexcept -> bool override;

  [[nodiscard]] auto GetPosition() const noexcept -> glm::vec3;
  [[nodiscard]] auto GetRotation() const noexcept -> glm::quat;
  [[nodiscard]] auto GetScale() const noexcept -> glm::vec3;
};

} // namespace oxygen::world
