load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library", "bdl_system")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
)

cc_library(
    name = "lib",
    hdrs = [
        "simple.hh",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bdl_system(
    name = "system",
    targets = {
        "esp32": "//cc/targets/esp32_xtensa_lx6:gcc",
        "hpc": "//cc/targets/linux:x86_64_clang",
    },
    deps = [
        ":lib",
    ],
)
