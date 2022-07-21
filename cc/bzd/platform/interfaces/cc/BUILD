load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "tcp",
    hdrs = [
        "tcp.hh",
    ],
    deps = [
        "//cc/bzd/container:string_view",
        "//tools/bdl/generators/cc/adapter:types"
    ],
    visibility = [
        "//cc/bzd/platform/interfaces:__pkg__",
    ],
)
