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
        "//cc/components/posix/proactor/epoll",
        "//cc/components/posix/stream/in",
        "//cc/components/posix/stream/out",
        "//cc/components/std/clock/steady",
        "//cc/components/std/clock/system",
    ],
)

cc_library(
    name = "linux",
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
