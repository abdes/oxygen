# ===-----------------------------------------------------------------------===#
# Distributed under the 3-Clause BSD License. See accompanying file LICENSE or
# copy at https://opensource.org/licenses/BSD-3-Clause.
# SPDX-License-Identifier: BSD-3-Clause
# ===-----------------------------------------------------------------------===#

load(
    "//oxygen:copts/configure_copts.bzl",
    "OXYGEN_DEFAULT_COPTS",
    "OXYGEN_DEFAULT_LINKOPTS",
    "OXYGEN_TEST_COPTS",
)

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "macros",
    srcs = ["detail/location_helper.cpp"],
    hdrs = [
        "macros.h",
    ],
    copts = OXYGEN_DEFAULT_COPTS,
    implementation_deps = ["@fmt"],
    linkopts = OXYGEN_DEFAULT_LINKOPTS,
    deps = ["@spdlog"],
)

cc_library(
    name = "delegating_sink",
    hdrs = [
        "delegating_sink.h",
    ],
    copts = OXYGEN_DEFAULT_COPTS,
    linkopts = OXYGEN_DEFAULT_LINKOPTS,
    deps = ["@spdlog"],
)

cc_library(
    name = "registry",
    srcs = [
        "detail/registered_logger.cpp",
        "detail/registered_logger.h",
        "detail/registry_impl.cpp",
        "detail/registry_impl.h",
        "registry.cpp",
    ],
    hdrs = [
        "registry.h",
    ],
    copts = OXYGEN_DEFAULT_COPTS,
    linkopts = OXYGEN_DEFAULT_LINKOPTS,
    deps = [
        ":delegating_sink",
        "//oxygen/base:macros",
        "@spdlog",
    ],
)

cc_library(
    name = "logging",
    hdrs = [
        "logging.h",
    ],
    copts = OXYGEN_DEFAULT_COPTS,
    linkopts = OXYGEN_DEFAULT_LINKOPTS,
    deps = [
        "registry",
        ":macros",
    ],
)

cc_test(
    name = "lopgging_test",
    size = "small",  # Other options: "medium", "large", "enormous"
    srcs = [
        "test/logging_test.cpp",
        "test/main.cpp",
    ],
    copts = OXYGEN_TEST_COPTS,
    linkopts = OXYGEN_DEFAULT_LINKOPTS,
    deps = [
        ":logging",
        "@googletest//:gtest",
        "@gsl",
        "@spdlog",
    ],
)
