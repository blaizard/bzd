load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/bzd/platform/esp32_xtensa_lx6/xthal_clock",
        "//cc/bzd/platform/generic/clock/null",
        "//cc/bzd/platform/generic/stream/stub",
        "//cc/bzd/platform/std/stream/out",
    ],
)

cc_library(
    name = "esp32_xtensa_lx6",
    srcs = select({
        "//tools/bazel_build/config:test": [],
        "//tools/bazel_build/config:binary": ["main.cc"],
    }),
    target_compatible_with = [
        "@esp32_xtensa_lx6_gcc//:compiler",
    ],
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
    deps = [
        ":composition",
        "//cc/bzd/platform/generic/panic/trap",
        "//cc/bzd/platform/std/atomic",
    ],
)

alias(
    name = "atomic",
    actual = "//cc/bzd/platform/std/atomic",
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
)
