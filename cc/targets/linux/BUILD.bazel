load("@bzd_bdl//:defs.bzl", "bdl_target")
load("@rules_cc//cc:defs.bzl", "cc_library")

filegroup(
    name = "composition",
    srcs = [
        "composition.bdl",
        "network.bdl",
    ],
    visibility = ["//cc/targets:__subpackages__"],
)

cc_library(
    name = "main",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:panic",
        "//cc/components/generic/executor",
        "//cc/components/generic/executor_profiler/memory",
        "//cc/components/linux/core",
        "//cc/components/linux/proactor/epoll",
        "//cc/components/posix/network/tcp:client",
        "//cc/components/posix/shmem",
        "//cc/components/posix/stack_trace",
        "//cc/components/posix/stream/in",
        "//cc/components/posix/stream/out",
        "//cc/components/std/clock/system_clock",
        "//cc/components/std/stream/out",
        "//cc/components/std/timer/steady_clock",
    ],
)

bdl_target(
    name = "auto",
    composition = [
        ":composition",
    ],
    language = "cc",
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":main",
    ],
)

bdl_target(
    name = "x86_64_gcc",
    parent = ":auto",
    platform = "@gcc//:platform-linux-x86_64",
    visibility = [
        "//visibility:public",
    ],
)

bdl_target(
    name = "x86_64_clang",
    parent = ":auto",
    platform = "@clang//:platform-linux-x86_64",
    visibility = [
        "//visibility:public",
    ],
)
