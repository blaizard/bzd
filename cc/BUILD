load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "bzd",
    hdrs = [
        "bzd.hh",
    ],
    deps = [
        "//cc/bzd"
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
