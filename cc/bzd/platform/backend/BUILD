load("//tools/bazel_build/rules:cc.bzl", "cc_library")

# Helper to map platform with their backend
PLATFORM_TO_TARGET = {
    "//tools/bazel_build/config:linux": "//cc/bzd/platform/linux",
    "@esp32_xtensa_lx6_gcc//:target": "//cc/bzd/platform/esp32_xtensa_lx6",
}

cc_library(
    name = "backend",
    srcs = [
        "backend.cc",
        "backend.hh",
    ],
    deps = [
        "//cc/bzd/platform:clock",
        "//cc/bzd/platform:stream",
    ] + select({platform: [target] for platform, target in PLATFORM_TO_TARGET.items()}),
    visibility = ["//cc/bzd/platform:__pkg__"],
)

cc_library(
    name = "atomic",
    visibility = ["//cc/bzd/platform:__pkg__"],
    deps = select({platform: [target + ":atomic"] for platform, target in PLATFORM_TO_TARGET.items()}),
)

cc_library(
    name = "setter",
    srcs = [
        "setter.cc",
    ],
    hdrs = [
        "backend.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:stream",
    ],
)
