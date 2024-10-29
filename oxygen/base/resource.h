//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#pragma once

#include "oxygen/base/resource_handle.h"

namespace oxygen {

/**
 * A graphics API agnostic POD structure representing different types of
 * resources that get linked to their counterparts on the core backend.
 */
class Resource {
public:
  ResourceHandle handle;
};

} // namespace oxygen
