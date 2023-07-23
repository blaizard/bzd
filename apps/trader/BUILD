load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//interfaces:tcp",
    ],
)

cc_library(
    name = "lib",
    hdrs = [
        "trader.hh",
    ],
    target_compatible_with = select({
        "@esp32//:toolchain": ["@platforms//:incompatible"],
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
