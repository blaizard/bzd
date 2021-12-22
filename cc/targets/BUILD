load("@rules_cc//cc:defs.bzl", "cc_library")

PLATFORM_TO_TARGET = {
    "//tools/bazel_build/config:linux": "//cc/targets/linux",
    "@esp32_xtensa_lx6_gcc//:target": "//cc/targets/esp32_xtensa_lx6",
    "@esp32_xtensa_lx6_clang//:target": "//cc/targets/esp32_xtensa_lx6",
}

cc_library(
    name = "targets",
    visibility = ["//cc/bzd/platform/backend:__pkg__"],
    deps = select({platform: [target] for platform, target in PLATFORM_TO_TARGET.items()}),
)

cc_library(
    name = "atomic",
    visibility = ["//cc/bzd/platform/backend:__pkg__"],
    deps = select({platform: [target + ":atomic"] for platform, target in PLATFORM_TO_TARGET.items()}),
)
