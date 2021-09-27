load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "backend",
    srcs = [
        "backend.cc",
    ],
    hdrs = [
        "backend.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:stream",
    ] + select({
        "//tools/bazel_build/config:linux": [
            "//cc/bzd/platform/linux:backend",
        ],
        "@esp32_xtensa_lx6_gcc//:target": [
            "//cc/bzd/platform/esp32_xtensa_lx6:backend",
        ],
    }),
)
