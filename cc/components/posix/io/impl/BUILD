load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "file_descriptor",
    hdrs = [
        "file_descriptor.hh",
    ],
    srcs = [
        "file_descriptor.cc",
    ],
    visibility = [
        "//cc/components/posix/io:__pkg__",
    ],
    deps = [
        "//cc/bzd/platform:types",
    ],
)
