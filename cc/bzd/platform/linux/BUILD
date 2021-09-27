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
    name = "backend",
    srcs = [
        "backend.cc",
    ],
    visibility = [
        "//cc/bzd/platform/backend:__pkg__",
    ],
    deps = [
        ":composition",
        "//cc/bzd/platform/generic/panic/throw",
        "//cc/bzd/platform/generic/stream/stub",
        "//cc/bzd/platform/posix/stack_trace",
        "//cc/bzd/platform/std/steady_clock",
        "//cc/bzd/platform/std/stream/out",
    ],
)
