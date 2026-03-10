load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

cc_library(
    name = "lib",
    srcs = [
        "main.cc",
    ],
    deps = [
        "//cc:bzd",
    ],
)

bzd_cc_binary(
    name = "channel",
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ],
)
