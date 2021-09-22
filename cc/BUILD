load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "includes",
    hdrs = [
        "bzd.hh",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "documentation",
    srcs = glob([
        "bzd/**/*.hh",
    ]),
    visibility = ["//visibility:public"],
)
