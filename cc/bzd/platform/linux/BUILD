load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/bzd/platform/generic/executor",
        "//cc/bzd/platform/linux/core",
        "//cc/bzd/platform/std/stream/out",
    ],
)

cc_library(
    name = "linux",
    srcs = select({
        "//tools/bazel_build/config:test": ["test.cc"],
        "//tools/bazel_build/config:binary": ["main.cc"],
    }),
    deps = [
        ":composition",
        "//cc/bzd/platform/generic/panic/throw",
        "//cc/bzd/platform/generic/stream/stub",
        "//cc/bzd/platform/posix/stack_trace",
        "//cc/bzd/platform/std/atomic",
        "//cc/bzd/platform/std/steady_clock",
        "//cc/bzd/platform/std/stream/out",
        "//cc/bzd/platform/std/system_clock",
    ],
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
)

alias(
    name = "atomic",
    actual = "//cc/bzd/platform/std/atomic",
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
)
