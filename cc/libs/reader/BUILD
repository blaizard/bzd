cc_library(
    name = "channel_reader",
    hdrs = [
        "channel_reader.hh"
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:ring_buffer",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
    ]
)

cc_library(
    name = "stream_reader",
    hdrs = [
        "stream_reader.hh"
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":channel_reader"
    ]
)
