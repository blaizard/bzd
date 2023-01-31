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
        "//cc/targets/linux:x86_64_clang": "hpc,old",
    },
    deps = [
        ":lib",
    ],
)
