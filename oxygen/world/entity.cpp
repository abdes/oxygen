//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "entity.h"

#include "oxygen/base/resource_table.h"
#include "transform.h"

namespace {
oxygen::ResourceTable<oxygen::world::Entity> entities(
    oxygen::world::resources::kEntity, 256);
}

auto oxygen::world::entity::CreateGameEntity(
    const Descriptor &entity_desc) -> Entity {
  // All game entities must have a transform component.
  assert(entity_desc.transform != nullptr);
  if (entity_desc.transform == nullptr) {
    return {};
  }

  // Create the new entity
  auto entity_id = entities.Insert({});
  if (!entity_id.IsValid()) {
    return {};
  }

  // Create the transform component from the given descriptor
  const auto transform = CreateTransform(*entity_desc.transform, entity_id);
  if (!transform.IsValid()) {
    entities.Erase(entity_id);
    return {};
  }
  assert(transform.GetId().Index() == entity_id.Index());
  assert(transform.GetId().Generation() == entity_id.Generation());

  Entity entity(entity_id);
  return entity;
}

auto oxygen::world::entity::RemoveGameEntity(const Entity &entity) -> size_t {
  // Save the transform because after the entity is removed, its handle is
  // reset.
  const auto entity_removed = entities.Erase(entity.GetEntityId());
  if (entity_removed != 0) {
    const auto transform_removed =
        transform::RemoveTransform(entity.GetTransform());
    assert(transform_removed == 1);
  }
  return entity_removed;
}

auto oxygen::world::Entity::GetTransform() const noexcept -> Transform {
  if (!IsValid()) {
    return {};
  }
  auto transform_id = GetTransformId();
  auto transform = Transform(transform_id);
  assert(transform.IsValid());
  return transform;
}
