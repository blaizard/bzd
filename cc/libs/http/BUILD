load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "ichannel_reader",
    hdrs = [
        "ichannel_reader.hh",
    ],
    deps = [
        "//cc/bzd/container:ring_buffer",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
    ],
)

cc_library(
    name = "http",
    hdrs = [
        "http.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":ichannel_reader",
        "//cc:bzd",
        "//cc/bzd/container:ostream_buffered",
        "//cc/bzd/utility/format:stream",
        "//cc/bzd/utility/synchronization:lock_guard",
        "//cc/bzd/utility/synchronization:mutex",
    ],
)
