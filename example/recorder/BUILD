load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/bzd/platform/interfaces:recorder"
    ]
)

cc_library(
    name = "lib",
    hdrs = [
        "recorder.hh",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "recorder",
    target = "//cc/targets/linux_2_executors:x86_64_clang",
    deps = [
        ":lib",
    ],
)

"""
bzd_cc_binary(
    name = "test",
    targets = {
        "hpc": "//cc/targets/linux_2_executors:x86_64_clang",
        "esp32": "@esp32_xtensa_lx6_gcc//:platform",
    },
    deps = [
        ":lib",
    ]
)

-> output 2 binaries
-> wrap the composition of linux_2_executors into "hpc" namespace, same for "esp32".
"""
