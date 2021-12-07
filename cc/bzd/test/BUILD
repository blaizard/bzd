load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

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
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":composition",
        "//cc/bzd/core:print",
        "//cc/bzd/core/async",
    ],
)

cc_library(
    name = "sync_point",
    hdrs = [
        "sync_point.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "inject_point",
    hdrs = [
        "inject_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_default_constructible",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "types",
    hdrs = [
        "types.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "utility",
    hdrs = [
        "utility.hh",
    ],
    visibility = ["//visibility:public"],
)
