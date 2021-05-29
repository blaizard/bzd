load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "includes",
    hdrs = [
        "bzd.h",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "documentation",
    srcs = glob([
        "bzd/**/*.h",
    ]),
    visibility = ["//visibility:public"],
)
