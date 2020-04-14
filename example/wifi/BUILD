load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "wifi",
    srcs = [
        "main.cpp",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc:bzd",
    ],
)

#bzd_pack(
#    name = "deploy",
#    binary = ":wifi",
#)
