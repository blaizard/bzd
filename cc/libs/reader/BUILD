load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "channel_reader",
    hdrs = [
        "channel_reader.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:ring_buffer",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/type_traits:container",
    ],
)

cc_library(
    name = "stream_reader",
    hdrs = [
        "stream_reader.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":channel_reader",
    ],
)
