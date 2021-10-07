load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/components/generic/executor",
        "//cc/components/generic/stream/stub",
        "//cc/components/linux/core",
        "//cc/components/std/clock/steady",
        "//cc/components/std/clock/system",
        "//cc/components/std/stream/out",
    ],
)

cc_library(
    name = "linux",
    srcs = select({
        "//tools/bazel_build/config:test": ["test.cc"],
        "//tools/bazel_build/config:binary": ["main.cc"],
    }),
    visibility = [
        "//cc/targets:__pkg__",
    ],
    deps = [
        ":composition",
        "//cc/components/generic/panic/throw",
        "//cc/components/posix/stack_trace",
        "//cc/components/std/atomic",
        "//tools/cc_test",
    ],
)

alias(
    name = "atomic",
    actual = "//cc/components/std/atomic",
    visibility = [
        "//cc/targets:__pkg__",
    ],
)
