load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

package(default_testonly = True)

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
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
        "//cc/bzd/core:print",
        "//cc/bzd/core/async",
        "//cc/bzd/meta:macro",
        "//cc/bzd/platform:clock",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/utility:numeric_limits",
        "//cc/bzd/utility/random:uniform_int_distribution",
        "//cc/bzd/utility/random:xorwow_engine",
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
