load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/components/generic/executor",
        "//cc/components/linux/core",
        "//cc/components/posix/shmem",
        "//cc/targets/linux:composition",
    ],
)

cc_library(
    name = "linux_2_executors",
    srcs = ["main.cc"],
    visibility = [
        "//cc/targets:__pkg__",
    ],
    deps = [
        ":composition",
        "//cc/components/generic/panic/throw",
        "//cc/components/posix/stack_trace",
    ],
)

platform(
    name = "x86_64_clang",
    constraint_values = [
        "//cc/targets:linux_2_executors",
    ],
    parents = ["@linux_x86_64_clang//:platform"],
    visibility = ["//visibility:public"],
)

platform(
    name = "x86_64_gcc",
    constraint_values = [
        "//cc/targets:linux_2_executors",
    ],
    parents = ["@linux_x86_64_gcc//:platform"],
    visibility = ["//visibility:public"],
)
