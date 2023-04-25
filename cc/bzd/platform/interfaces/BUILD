load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "clock",
    hdrs = [
        "clock.hh",
    ],
    visibility = [
        "//interfaces:__pkg__",
    ],
    deps = [
        "//cc/bzd/core:units",
        "//cc/bzd/core/async",
        "//tools/bdl/generators/cc/adapter:types",
    ],
)

cc_library(
    name = "core",
    hdrs = [
        "core.hh",
    ],
    visibility = [
        "//interfaces:__pkg__",
    ],
    deps = [
        "//cc/bzd/container:string_view",
        "//tools/bdl/generators/cc/adapter:types",
    ],
)

cc_library(
    name = "gateway",
    hdrs = [
        "gateway.hh",
    ],
    visibility = [
        "//interfaces:__pkg__",
    ],
    deps = [
        "//cc/bzd/core/async",
        "//tools/bdl/generators/cc/adapter:types",
    ],
)

cc_library(
    name = "tcp",
    hdrs = [
        "tcp.hh",
    ],
    visibility = [
        "//interfaces:__pkg__",
    ],
    deps = [
        "//cc/bzd/container:string_view",
        "//cc/bzd/core/async",
        "//tools/bdl/generators/cc/adapter:types",
    ],
)

cc_library(
    name = "timer",
    hdrs = [
        "timer.hh",
    ],
    visibility = [
        "//interfaces:__pkg__",
    ],
    deps = [
        "//cc/bzd/core:units",
        "//cc/bzd/core/async",
        "//tools/bdl/generators/cc/adapter:types",
    ],
)
