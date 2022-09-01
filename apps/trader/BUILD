load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/bzd/platform/interfaces:tcp",
    ],
)

cc_library(
    name = "lib",
    hdrs = [
        "trader.hh",
    ],
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        ":composition",
        "//cc:bzd",
        "//cc/libs/http",
    ],
)

bzd_cc_binary(
    name = "trader",
    deps = [
        ":lib",
    ],
)
