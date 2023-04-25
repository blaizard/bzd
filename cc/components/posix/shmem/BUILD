load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//interfaces:gateway",
    ],
)

cc_library(
    name = "shmem",
    hdrs = [
        "shmem.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
    ],
)
