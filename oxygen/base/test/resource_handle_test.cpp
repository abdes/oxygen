//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "oxygen/base/resource_handle.h"

#include "gtest/gtest.h"

using oxygen::ResourceHandle;

// NOLINTNEXTLINE
TEST(ResourceHandleTest, InvalidHandle) {
  const ResourceHandle handle;
  ASSERT_FALSE(handle.IsValid());
}

// NOLINTNEXTLINE
TEST(ResourceHandleTest, ValidHandle) {
  const ResourceHandle handle(1U, 0x04);
  EXPECT_EQ(handle.Index(), 1U);
  EXPECT_EQ(handle.ResourceType(), 0x04);
  EXPECT_EQ(handle.Generation(), 0);
}

// NOLINTNEXTLINE
TEST(ResourceHandleTest, Comparison) {
  // Arrange & Act
  const ResourceHandle handle1(1U, 0x04);
  const ResourceHandle handle2(1U, 0x04);
  const ResourceHandle handle3(2U, 0x04);

  // Assert
  EXPECT_TRUE(handle1 == handle2);
  EXPECT_TRUE(handle1 < handle3);
  EXPECT_TRUE(handle1 != handle3);
}

// NOLINTNEXTLINE
TEST(ResourceHandleTest, GetHandle) {
  const ResourceHandle handle(1U, 0x04);
  const ResourceHandle::HandleT theHandle = handle.Handle();
  EXPECT_EQ(theHandle, 0x0004'0000'0000'0001);
}

// NOLINTNEXTLINE
TEST(ResourceHandleTest, NewGeneration) {
  ResourceHandle handle(1U, 0x03);
  ASSERT_EQ(handle.Generation(), 0);
  for (ResourceHandle::GenerationT gen = 0;
       gen < ResourceHandle::kGenerationMax; gen++) {
    handle.NewGeneration();
    ASSERT_EQ(handle.Index(), 1U);
    ASSERT_EQ(handle.ResourceType(), 0x03);
    ASSERT_EQ(handle.Generation(), gen + 1);
  }
#if defined(ASAP_IS_DEBUG_BUILD)
  ASSERT_DEATH(handle.NewGeneration(), "");
#else
  handle.NewGeneration();
  ASSERT_EQ(handle.Generation(), 0);
#endif
}

// NOLINTNEXTLINE
TEST(ResourceHandleTest, SetResourceType) {
  ResourceHandle handle(1U);
  EXPECT_EQ(handle.ResourceType(), ResourceHandle::kTypeNotInitialized);
  handle.SetResourceType(0x12);
  EXPECT_EQ(handle.ResourceType(), 0x12);
}

// NOLINTNEXTLINE
TEST(ResourceHandleTest, SetIndex) {
  ResourceHandle handle;
  handle.SetIndex(0);
  EXPECT_EQ(handle.Index(), 0);
  static constexpr ResourceHandle::IndexT kValidIndex = 12345;
  handle.SetIndex(kValidIndex);
  EXPECT_EQ(handle.Index(), kValidIndex);
}

// NOLINTNEXTLINE
TEST(ResourceHandleTest, SetFree) {
  ResourceHandle handle(1U, 0x03);
  handle.NewGeneration();
  EXPECT_EQ(handle.Index(), 1U);
  EXPECT_EQ(handle.ResourceType(), 0x03);
  EXPECT_EQ(handle.Generation(), 1);
  ASSERT_FALSE(handle.IsFree());
  handle.SetFree(true);
  EXPECT_TRUE(handle.IsFree());
  EXPECT_EQ(handle.Index(), 1U);
  EXPECT_EQ(handle.ResourceType(), 0x03);
  ASSERT_EQ(handle.Generation(), 1);
  handle.SetFree(false);
  ASSERT_FALSE(handle.IsFree());
  EXPECT_EQ(handle.Index(), 1U);
  EXPECT_EQ(handle.ResourceType(), 0x03);
  ASSERT_EQ(handle.Generation(), 1);
}
