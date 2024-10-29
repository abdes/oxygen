//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include <type_traits>
#include <cassert>

#include "oxygen/base/types.h"

namespace oxygen {

/*
A graphics API agnostic POD structure representing different types of resources
that get linked to their counterparts on the core backend.

The handle is used as an alternative to pointers / associative container lookup
to achieve several enhancements:

1. Store data in a contiguous block of memory.
2. Create an associative mapping between the application view of the resource
   and the actual data on the core side, while ensuring O(1) lookups, O(1)
   insertions and O(1) removals for maximum efficiency.

The handle is a 64-bit value, so there is no additional overhead compared to a
pointer on 64-bit platforms.

The 64-bit value is laid out in the following way, with the order of the fields
being important for sorting prioritized by the free status, then resource type,
then generation, and finally index.

   1       15                16                         32 X<-    type    -> <-
   gen    -> <------------- index ------------->
   ........ ........ ........ ........ ........ ........ ........ ........

The most significant bit of the handle is reserved (used for implementation of
the handle lookup table). When set, the handle is part of freelist managed by
the lookup table and can be allocated for a new resource. Otherwise, the handle
is active. This gives us an embedded singly linked list within the lookup table
costing just 1 bit in the handle. As long as we store the front index of the
freelist separately, it is an O(1) operation to find the next available slot and
maintain the singly linked list.

The next most significant bits of the handle hold the resource type. This is
extra information, that can introduce an element of type safety in the
application or be used for special handling of resources by type.

The generation field is used as a safety mechanism to detect when a stale handle
is trying to access data that has since been overwritten in the corresponding
slot. Every time a slot in the lookup table is removed, the generation
increments. Handle lookups assert that the generations match.

The remaining bits are simply an index into an array for that specific resource
type inside the Render Device.
*/
class ResourceHandle
{
 public:
  using HandleT = uint64_t;

 private:
  static constexpr uint8_t kHandleBits = sizeof(HandleT) * 8;
  static constexpr uint8_t kGenerationBits{16};
  static constexpr uint8_t kResourceTypeBits{15};
  static constexpr uint8_t kIndexBits{
      kHandleBits - kGenerationBits - kResourceTypeBits - 1};

  static constexpr HandleT kHandleMask = static_cast<HandleT>(-1);
  static constexpr HandleT kIndexMask = (HandleT{1} << kIndexBits) - 1;
  static constexpr HandleT kGenerationMask =
      (HandleT{1} << kGenerationBits) - 1;
  static constexpr HandleT kResourceTypeMask =
      (HandleT{1} << kResourceTypeBits) - 1;

 public:
  using GenerationT = std::conditional_t<
      kGenerationBits <= 16,
      std::conditional_t<kGenerationBits <= 8, uint8_t, uint16_t>,
      uint32_t>;
  static constexpr GenerationT kGenerationMax = kGenerationMask;

  using ResourceTypeT = std::conditional_t<
      kGenerationBits <= 16,
      std::conditional_t<kGenerationBits <= 8, uint8_t, uint16_t>,
      uint32_t>;
  static constexpr GenerationT kTypeNotInitialized = kResourceTypeMask;
  static constexpr GenerationT kResourceTypeMax = kResourceTypeMask;

  using IndexT = std::conditional_t<kIndexBits <= 32, uint32_t, uint64_t>;
  static constexpr IndexT kIndexMax = kIndexMask;
  static constexpr IndexT kInvalidIndex = kIndexMax;

  ResourceHandle();

  virtual ~ResourceHandle() = default;

  explicit ResourceHandle(
      IndexT index,
      ResourceTypeT type = kTypeNotInitialized);

  ResourceHandle(const ResourceHandle&) = default;
  auto operator=(const ResourceHandle&) -> ResourceHandle& = default;

  ResourceHandle(ResourceHandle&&) = default;
  auto operator=(ResourceHandle&&) -> ResourceHandle& = default;

  auto operator==(const ResourceHandle& rhs) const -> bool;
  auto operator!=(const ResourceHandle& rhs) const -> bool;
  auto operator<(const ResourceHandle& rhs) const -> bool;

  [[nodiscard]] auto Handle() const -> HandleT;

  [[nodiscard]] auto IsValid() const -> bool;

  void Invalidate();

  [[nodiscard]] auto Index() const -> IndexT;

  void SetIndex(IndexT index);

  [[nodiscard]] auto Generation() const -> GenerationT;

  void NewGeneration();

  [[nodiscard]] auto ResourceType() const -> ResourceTypeT;

  void SetResourceType(ResourceTypeT type);

  [[nodiscard]] auto IsFree() const -> bool;

  void SetFree(bool flag);

 private:
  HandleT handle_{kHandleMask};

  void SetGeneration(GenerationT generation);

  static constexpr HandleT kResourceTypeSetMask =
      ((HandleT{1} << (kIndexBits + kGenerationBits)) - 1) |
      (HandleT{1} << (kIndexBits + kGenerationBits + kResourceTypeBits));
  static constexpr HandleT kGenerationSetMask =
      (kHandleMask << (kIndexBits + kGenerationBits)) | kIndexMask;
  static constexpr HandleT kIndexSetMask = kHandleMask << kIndexBits;

  static_assert((kHandleBits - kGenerationBits - kResourceTypeBits) > 0);
  static_assert(sizeof(GenerationT) * 8 >= kGenerationBits);
  static_assert(sizeof(ResourceTypeT) * 8 >= kResourceTypeBits);
  static_assert(sizeof(IndexT) * 8 >= kIndexBits);
};

// -----------------------------------------------------------------------------

inline ResourceHandle::ResourceHandle(
    const IndexT index,
    const ResourceTypeT type)
{
  SetIndex(index);
  SetResourceType(type);
  SetGeneration(0);
  SetFree(false);
}

inline ResourceHandle::ResourceHandle()
{
  SetGeneration(0);
  SetFree(false);
}

inline auto ResourceHandle::operator==(const ResourceHandle& rhs) const -> bool
{
  return handle_ == rhs.handle_;
}

inline auto ResourceHandle::operator!=(const ResourceHandle& rhs) const -> bool
{
  return handle_ != rhs.handle_;
}

inline auto ResourceHandle::operator<(const ResourceHandle& rhs) const -> bool
{
  return handle_ < rhs.handle_;
}

inline auto ResourceHandle::IsValid() const -> bool
{
  return Index() != kInvalidIndex;
}

inline void ResourceHandle::Invalidate() { this->handle_ = kHandleMask; }

inline auto ResourceHandle::Handle() const -> HandleT { return handle_; }

inline auto ResourceHandle::Index() const -> IndexT
{
  return handle_ & kIndexMask;
}

inline void ResourceHandle::SetIndex(const IndexT index)
{
  assert(index <= kIndexMax);  // max value is invalid
  handle_ = (handle_ & kIndexSetMask) |
            // NOLINTNEXTLINE(clang-diagnostic-tautological-type-limit-compare)
            (index <= kIndexMax ? index : kInvalidIndex);
}

inline auto ResourceHandle::ResourceType() const -> ResourceTypeT
{
  return static_cast<ResourceTypeT>(
      (handle_ >> (kIndexBits + kGenerationBits)) & kResourceTypeMask);
}

inline void ResourceHandle::SetResourceType(const ResourceTypeT type)
{
  assert(type <= kResourceTypeMax);  // max value is not-initialized
  handle_ = (handle_ & kResourceTypeSetMask) |
            (static_cast<HandleT>(type) << (kIndexBits + kGenerationBits));
}

inline auto ResourceHandle::Generation() const -> GenerationT
{
  return (handle_ >> (kIndexBits)) & kGenerationMask;
}

inline void ResourceHandle::SetGeneration(GenerationT generation)
{
  assert(generation <= kGenerationMax);
  // Wrap around
  // NOLINTNEXTLINE(clang-diagnostic-tautological-type-limit-compare)
  if (generation > kGenerationMax) {
    generation = 0;
  }
  handle_ = (handle_ & kGenerationSetMask) |
            (static_cast<HandleT>(generation) << kIndexBits);
}

inline void ResourceHandle::NewGeneration()
{
  auto current_generation = Generation();
  assert(current_generation <= kGenerationMax);
  auto new_generation{(current_generation == kGenerationMax) ? 0 : current_generation + 1U};
  // Wrap around
  handle_ = (handle_ & kGenerationSetMask) |
            (static_cast<HandleT>(new_generation) << kIndexBits);
}

inline auto ResourceHandle::IsFree() const -> bool
{
  return (handle_ & (HandleT{1} << (kHandleBits - 1))) != 0;
}

inline void ResourceHandle::SetFree(const bool flag)
{
  handle_ &= ((HandleT{1} << (kHandleBits - 1)) - 1);
  if (flag) {
    handle_ |= (HandleT{1} << (kHandleBits - 1));
  }
}

}  // namespace oxygen
