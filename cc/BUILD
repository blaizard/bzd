load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "bzd",
    hdrs = [
        "bzd.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd",
    ],
)

filegroup(
    name = "documentation",
    srcs = glob([
        "bzd/**/*.hh",
    ]),
    visibility = ["//visibility:public"],
)
