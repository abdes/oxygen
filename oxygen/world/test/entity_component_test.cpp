//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "gtest/gtest.h"

#include "oxygen/world/entity.h"
#include "oxygen/world/transform.h"

using oxygen::world::Entity;
using oxygen::world::EntityDescriptor;
using oxygen::world::EntityId;
using oxygen::world::Transform;
using oxygen::world::TransformDescriptor;
using oxygen::world::TransformId;
using oxygen::world::entity::CreateGameEntity;
using oxygen::world::entity::RemoveGameEntity;

// NOLINTNEXTLINE
TEST(EntityComponentTest, CanCreateAndRemoveEntity) {
  TransformDescriptor tranform_desc{};
  EntityDescriptor entity_desc{
      .transform = &tranform_desc,
  };

  auto entity = CreateGameEntity(entity_desc);

  ASSERT_TRUE(entity.IsValid());

  auto removed = RemoveGameEntity(entity);
  ASSERT_EQ(removed, 1);
}

// NOLINTNEXTLINE
TEST(EntityComponentTest, CreateEntityCreatesTransform) {
  TransformDescriptor tranform_desc{};
  EntityDescriptor entity_desc{
      .transform = &tranform_desc,
  };

  auto entity = CreateGameEntity(entity_desc);

  ASSERT_TRUE(entity.IsValid());
  auto transform = entity.GetTransform();
  ASSERT_TRUE(transform.IsValid());
}

// NOLINTNEXTLINE
TEST(EntityComponentTest, RemoveEntityRemovesTransform) {
  TransformDescriptor tranform_desc{};
  EntityDescriptor entity_desc{
      .transform = &tranform_desc,
  };

  auto entity = CreateGameEntity(entity_desc);
  ASSERT_TRUE(entity.IsValid());
  auto transform = entity.GetTransform();

  auto removed = RemoveGameEntity(entity);
  ASSERT_EQ(removed, 1);
  ASSERT_FALSE(transform.IsValid());
}
