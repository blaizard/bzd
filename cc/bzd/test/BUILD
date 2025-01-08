load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_testonly = True)

bdl_library(
    name = "runner",
    srcs = [
        "runner.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//interfaces:stream",
        "//interfaces:timer",
    ],
)

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        ":runner",
    ],
)

cc_library(
    name = "multithread",
    hdrs = [
        "multithread.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":test",
        "//cc/libs/pthread",
    ],
)

cc_library(
    name = "test_without_composition",
    srcs = [
        "test.cc",
    ],
    hdrs = [
        "test.hh",
        "test_internal.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":runner",
        "//cc/bzd/container:ring_buffer",
        "//cc/bzd/core:print",
        "//cc/bzd/core/async",
        "//cc/bzd/meta:macro",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/type_traits:remove_cvref",
        "//cc/bzd/utility:numeric_limits",
        "//cc/bzd/utility/random:uniform_int_distribution",
        "//cc/bzd/utility/random:xorwow_engine",
        "//cc/bzd/utility/ranges:begin",
        "//cc/bzd/utility/ranges:end",
        "//interfaces:timer",
    ],
)

cc_library(
    name = "test",
    srcs = [
        "test.cc",
    ],
    hdrs = [
        "test.hh",
        "test_internal.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":composition",
        ":test_without_composition",
    ],
)

cc_library(
    name = "sync_barrier",
    hdrs = [
        "sync_barrier.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/platform:atomic",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "sync_point",
    hdrs = [
        "sync_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:atomic",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "exec_point",
    testonly = False,
    hdrs = [
        "exec_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
    ],
)

cc_library(
    name = "inject_point",
    testonly = False,
    hdrs = [
        "inject_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:derived_from",
        "//cc/bzd/type_traits:is_default_constructible",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "utility",
    hdrs = [
        "utility.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
    ],
)
