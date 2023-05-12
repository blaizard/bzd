load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "http",
    hdrs = [
        "http.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc:bzd",
        "//cc/bzd/container:ostream_buffered",
        "//cc/bzd/utility/pattern:to_stream",
        "//cc/bzd/utility/pattern/from_string.hh",
        "//cc/bzd/utility/synchronization:lock_guard",
        "//cc/bzd/utility/synchronization:mutex",
        "//cc/libs/reader:stream_reader",
    ],
)
