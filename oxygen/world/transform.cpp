//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "oxygen/base/resource_table.h"

#include "transform.h"

namespace {
oxygen::ResourceTable<oxygen::world::Transform> transforms(
    oxygen::world::resources::kTransform, 256);

oxygen::ResourceTable<glm::vec3> positions(
    oxygen::world::resources::kTransform, 256);
oxygen::ResourceTable<glm::quat> rotations(
    oxygen::world::resources::kTransform, 256);
oxygen::ResourceTable<glm::vec3> scales(
    oxygen::world::resources::kTransform, 256);
} // namespace

auto oxygen::world::transform::CreateTransform(
    TransformDescriptor &transform_desc,
    const EntityId &entity_id) -> Transform {

  const auto transform_id = transforms.Insert({});
  assert(transform_id.Index() == entity_id.Index());

  const auto position_id = positions.Insert(transform_desc.position);
  assert(position_id.Index() == entity_id.Index());

  const auto rotation_id = rotations.Insert(transform_desc.rotation);
  assert(rotation_id.Index() == entity_id.Index());

  const auto scale_id = scales.Insert(transform_desc.scale);
  assert(scale_id.Index() == entity_id.Index());

  Transform transform(transform_id);
  return Transform(transform_id);
}

auto oxygen::world::transform::RemoveTransform(
    const Transform &transform) -> size_t {
  auto transform_removed = transforms.Erase(transform.GetId());
  assert(transform_removed != 0);
  if (transform_removed != 0) {
    transform_removed = positions.Erase(transform.GetId());
    assert(transform_removed == 1);
    transform_removed = rotations.Erase(transform.GetId());
    assert(transform_removed == 1);
    transform_removed = scales.Erase(transform.GetId());
    assert(transform_removed == 1);
  }
  return transform_removed;
}

auto oxygen::world::Transform::GetPosition() const noexcept -> glm::vec3 {
  assert(IsValid());
  return positions.ItemAt(GetId());
}

auto oxygen::world::Transform::GetRotation() const noexcept -> glm::quat {
  assert(IsValid());
  return rotations.ItemAt(GetId());
}

auto oxygen::world::Transform::GetScale() const noexcept -> glm::vec3 {
  assert(IsValid());
  return scales.ItemAt(GetId());
}

auto oxygen::world::Transform::IsValid() const noexcept -> bool {
  return Resource::IsValid() && transforms.Contains(GetId());
}
