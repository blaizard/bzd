load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "file_descriptor",
    srcs = [
        "file_descriptor.cc",
    ],
    hdrs = [
        "file_descriptor.hh",
    ],
    visibility = [
        "//cc/components/posix/io:__pkg__",
    ],
    deps = [
        "//cc/bzd/platform:types",
    ],
)
