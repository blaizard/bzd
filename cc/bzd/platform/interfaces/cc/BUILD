load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "tcp",
    hdrs = [
        "tcp.hh",
    ],
    visibility = [
        "//cc/bzd/platform/interfaces:__pkg__",
    ],
    deps = [
        "//cc/bzd/container:string_view",
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
        "//cc/bzd/platform/interfaces:__pkg__",
    ],
    deps = [
        "//cc/bzd/container:string_view",
        "//tools/bdl/generators/cc/adapter:types",
    ],
)
