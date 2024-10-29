//===----------------------------------------------------------------------===//
// Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
// copy at https://opensource.org/licenses/BSD-3-Clause.
// SPDX-License-Identifier: BSD-3-Clause
//===----------------------------------------------------------------------===//

#include <string>

#include "fmt/format.h"

#ifndef NDEBUG
namespace oxygen::log::detail {

/*!
 * @brief Make a string with the source code file name and line number at which
 * the log message was produced.
 * @param file source code file name.
 * @param line source code line number.
 * @return a formatted string with the file name and line number.
 */
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto FormatFileAndLine(char const *file, char const *line) -> std::string {
  constexpr static int FILE_MAX_LENGTH = 40;
  std::string fstr(file);
  if (fstr.length() > FILE_MAX_LENGTH) {
    constexpr auto shortened_prefix_length = 10;
    fstr = fstr.substr(0, shortened_prefix_length - 3)
               .append("...")
               .append(fstr.substr(
                   fstr.length() - FILE_MAX_LENGTH + shortened_prefix_length,
                   FILE_MAX_LENGTH - shortened_prefix_length));
  }
  constexpr auto line_number_length = 5;
  return fmt::format(
      "[{:>{}}:{:0>{}}] ", fstr, FILE_MAX_LENGTH, line, line_number_length);
}

} // namespace oxygen::log::detail
#endif // NDEBUG
