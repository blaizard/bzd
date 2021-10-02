load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "backend",
    srcs = [
        "backend.hh",
        "backend.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:stream",
        "//cc/bzd/platform:clock",
    ] + select({
        "//tools/bazel_build/config:linux": [
            "//cc/bzd/platform/linux:backend",
        ],
        "@esp32_xtensa_lx6_gcc//:target": [
            "//cc/bzd/platform/esp32_xtensa_lx6:backend",
        ],
    }),
)

cc_library(
    name = "setter",
    hdrs = [
        "backend.hh",
    ],
    srcs = [
        "setter.cc",
    ],
    deps = [
        "//cc/bzd/platform:stream",
    ],
    visibility = ["//visibility:public"],
)