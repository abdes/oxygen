//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include "oxygen/base/resource_table.h"

#include <ranges>

#include "gtest/gtest.h"

#include "oxygen/base/macros.h"

using oxygen::HandleSet;
using oxygen::ResourceHandle;
using oxygen::ResourceTable;

namespace {

struct Item {
  std::string value;

  bool constructed{false};
  bool copyConstructed{false};
  bool moveConstructed{false};

  explicit Item(std::string str) : value(std::move(str)), constructed(true) {
  }

  Item(const Item &o)
      : value(o.value), constructed(o.constructed), copyConstructed(true),
        moveConstructed(o.moveConstructed) {
  }

  Item(Item &&o) noexcept
      : value(std::move(o.value)), constructed(o.constructed),
        copyConstructed(o.copyConstructed), moveConstructed(true) {
  }

  Item &operator=(const Item &other) = default;
  Item &operator=(Item &&other) noexcept = default;
};

} // namespace
// NOLINTNEXTLINE
TEST(ResourceTableTest, EmptyTable) {
  static constexpr size_t kCapacity{10};
  static constexpr ResourceHandle::ResourceTypeT kItemType{1};

  const ResourceTable<void *> table(kItemType, kCapacity);
  EXPECT_TRUE(table.IsEmpty());
  EXPECT_EQ(table.Size(), 0);
  EXPECT_EQ(table.Capacity(), kCapacity);

  ResourceHandle handle(0, kItemType);
  EXPECT_FALSE(table.Contains(handle));
  handle.SetFree(false);
  EXPECT_FALSE(table.Contains(handle));
  handle.Invalidate();
  EXPECT_FALSE(table.Contains(handle));
}

// NOLINTNEXTLINE
TEST(ResourceTableTest, InsertItem) {
  static constexpr size_t kCapacity{10};
  static constexpr ResourceHandle::ResourceTypeT kItemType{1};

  ResourceTable<Item> table(kItemType, kCapacity);

  {
    Item item("Copied");
    auto handle = table.Insert(item);
    EXPECT_TRUE(handle.IsValid());
    EXPECT_EQ(table.Size(), 1);
    EXPECT_EQ(handle.ResourceType(), kItemType);
    auto &itemInTable = table.ItemAt(handle);
    EXPECT_TRUE(itemInTable.constructed);
    EXPECT_TRUE(itemInTable.copyConstructed);
    EXPECT_FALSE(itemInTable.moveConstructed);
  }

  {
    const Item const_item("Const Copied");
    auto handle = table.Insert(const_item);
    EXPECT_EQ(table.Size(), 2);
    auto &itemInTable = table.ItemAt(handle);
    EXPECT_TRUE(itemInTable.constructed);
    EXPECT_TRUE(itemInTable.copyConstructed);
    EXPECT_FALSE(itemInTable.moveConstructed);
  }

  {
    Item moved_item("Moved");
    auto handle = table.Insert(std::move(moved_item));
    EXPECT_EQ(table.Size(), 3);
    auto &itemInTable = table.ItemAt(handle);
    EXPECT_TRUE(itemInTable.constructed);
    EXPECT_FALSE(itemInTable.copyConstructed);
    EXPECT_TRUE(itemInTable.moveConstructed);
  }
}

// NOLINTNEXTLINE
TEST(ResourceTableTest, EmplaceItem) {
  static constexpr size_t kCapacity{10};
  static constexpr ResourceHandle::ResourceTypeT kItemType{1};

  ResourceTable<Item> table(kItemType, kCapacity);

  {
    auto handle = table.Emplace("Constructed");
    EXPECT_TRUE(handle.IsValid());
    EXPECT_EQ(table.Size(), 1);
    EXPECT_EQ(handle.ResourceType(), kItemType);
    auto &itemInTable = table.ItemAt(handle);
    EXPECT_TRUE(itemInTable.constructed);
    EXPECT_FALSE(itemInTable.copyConstructed);
    EXPECT_TRUE(itemInTable.moveConstructed);
  }
  {
    auto handle = table.Emplace(Item("Constructed"));
    EXPECT_TRUE(handle.IsValid());
    EXPECT_EQ(table.Size(), 2);
    EXPECT_EQ(handle.ResourceType(), kItemType);
    auto &itemInTable = table.ItemAt(handle);
    EXPECT_TRUE(itemInTable.constructed);
    EXPECT_TRUE(itemInTable.copyConstructed);
    EXPECT_TRUE(itemInTable.moveConstructed);
  }

  {
    const Item item("Copied");
    auto handle = table.Emplace(item);
    EXPECT_EQ(table.Size(), 3);
    auto &itemInTable = table.ItemAt(handle);
    EXPECT_TRUE(itemInTable.constructed);
    EXPECT_TRUE(itemInTable.copyConstructed);
  }

  {
    Item moved_item("Move Constructed");
    auto handle = table.Insert(std::move(moved_item));
    EXPECT_EQ(table.Size(), 4);
    auto &itemInTable = table.ItemAt(handle);
    EXPECT_TRUE(itemInTable.constructed);
    EXPECT_FALSE(itemInTable.copyConstructed);
    EXPECT_TRUE(itemInTable.moveConstructed);
  }
}

// NOLINTNEXTLINE
TEST(ResourceTableTest, EraseItemCallsItsDestructor) {
  static constexpr size_t kCapacity{10};
  static constexpr ResourceHandle::ResourceTypeT kItemType{1};

  static auto item_destroyed{false};
  struct Item {
    explicit Item(std::string a_value = "value") : value(std::move(a_value)) {
    }
    ~Item() {
      item_destroyed = true;
    }

    OXYGEN_DEFAULT_COPYABLE(Item)
    OXYGEN_DEFAULT_MOVABLE(Item)

    std::string value;
  };
  ResourceTable<Item> table(kItemType, kCapacity);

  const auto handle = table.Emplace();
  const auto erased = table.Erase(handle);
  EXPECT_EQ(erased, 1);
  EXPECT_TRUE(item_destroyed);
  EXPECT_EQ(table.Size(), 0);
}

// NOLINTNEXTLINE
TEST(ResourceTableTest, SparseArrayWithHoles) {
  static constexpr size_t kCapacity{3};
  static constexpr ResourceHandle::ResourceTypeT kItemType{1};

  ResourceTable<std::string> table(kItemType, kCapacity);
  const auto handle_1 = table.Emplace("1");
  auto handle_2 = table.Emplace("2");
  const auto handle_3 = table.Emplace("3");
  EXPECT_EQ(table.Size(), 3);
  EXPECT_EQ(table.Capacity(), 3);
  EXPECT_EQ(table.Erase(handle_2), 1);
  EXPECT_FALSE(table.Contains(handle_2));
  EXPECT_EQ(table.Capacity(), 3);
  handle_2 = table.Emplace("2");
  EXPECT_TRUE(table.Contains(handle_2));
  EXPECT_EQ(table.Size(), 3);
  EXPECT_EQ(table.Capacity(), 3);
  const auto handle_4 = table.Emplace("4");
  EXPECT_TRUE(table.Contains(handle_4));
  EXPECT_EQ(table.Size(), 4);
  EXPECT_EQ(table.Capacity(), 4);
}

// NOLINTNEXTLINE
TEST(ResourceTableTest, Defragment) {
  static constexpr size_t kCapacity{5};
  static constexpr ResourceHandle::ResourceTypeT kItemType{1};

  ResourceTable<int> table(kItemType, kCapacity);

  const auto handle_43 = table.Emplace(43);
  const auto handle_42 = table.Emplace(42);
  table.Erase(handle_43);
  table.Emplace(41);
  table.Erase(handle_42);

  table.Emplace(45);
  table.Emplace(44);

  const size_t swaps =
      table.Defragment([](const int &a, const int &b) { return a < b; });

  EXPECT_EQ(swaps, 2);
}

class ResourceTableTestPreFilled : public testing::Test {
public:
  static constexpr size_t kCapacity{3};
  static constexpr ResourceHandle::ResourceTypeT kItemType{1};

  ResourceTableTestPreFilled() : table_(kItemType, kCapacity) {
  }

protected:
  void SetUp() override {
    for (auto index = 1U; index <= kCapacity; index++) {
      handles_.push_back(table_.Emplace(std::to_string(index)));
    }
    std::ranges::for_each(handles_,
        [this](const auto &handle) { EXPECT_TRUE(table_.Contains(handle)); });
  }

  ResourceTable<std::string> table_;
  HandleSet handles_;
};

// NOLINTNEXTLINE
TEST_F(ResourceTableTestPreFilled, EraseItems) {
  table_.EraseItems(handles_);
  EXPECT_TRUE(table_.IsEmpty());
  EXPECT_EQ(table_.Size(), 0);
  EXPECT_EQ(table_.Capacity(), kCapacity);
}

// NOLINTNEXTLINE
TEST_F(ResourceTableTestPreFilled, Reset) {
  table_.Reset();
  EXPECT_TRUE(table_.IsEmpty());
  EXPECT_EQ(table_.Size(), 0);
  EXPECT_EQ(table_.Capacity(), kCapacity);
  const auto handle = table_.Emplace("after_reset");
  EXPECT_EQ(handle.Generation(), 0);
}

// NOLINTNEXTLINE
TEST_F(ResourceTableTestPreFilled, Clear) {
  table_.Clear();
  EXPECT_TRUE(table_.IsEmpty());
  EXPECT_EQ(table_.Size(), 0);
  EXPECT_EQ(table_.Capacity(), kCapacity);
  const auto handle = table_.Emplace("after_reset");
  EXPECT_GT(handle.Generation(), 0);
}
