load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "esp32_xtensa_lx6",
    srcs = select({
        "//tools/bazel_build/config:test": ["test.cc"],
        "//tools/bazel_build/config:binary": ["main.cc"],
    }),
    target_compatible_with = [
        "@esp32_xtensa_lx6_gcc//:compiler",
    ],
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
    deps = [
        "//cc/bzd/platform/esp32_xtensa_lx6/xthal_clock",
        "//cc/bzd/platform/generic/clock/null",
        "//cc/bzd/platform/generic/panic/trap",
        "//cc/bzd/platform/generic/stream/stub",
        "//cc/bzd/platform/std/atomic",
        "//cc/bzd/platform/std/stream/out",
    ],
)

alias(
    name = "atomic",
    actual = "//cc/bzd/platform/std/atomic",
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
)
