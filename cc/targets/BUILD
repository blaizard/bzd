load("@rules_cc//cc:defs.bzl", "cc_library")

PLATFORM_TO_TARGET = {
    "@platforms//os:linux": "//cc/targets/linux",
    "@//tools/bazel_build/platforms/isa:xtensa_lx6": "//cc/targets/esp32_xtensa_lx6",
}

cc_library(
    name = "targets",
    visibility = ["//cc/bzd/platform/backend:__pkg__"],
    deps = select({platform: [target] for platform, target in PLATFORM_TO_TARGET.items()}),
)
