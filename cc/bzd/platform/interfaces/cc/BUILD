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
        "//tools/bdl/generators/cc/adapter:types",
    ],
)
