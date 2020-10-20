load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "x86_64_cdecl",
    srcs = glob([
        "*.S",
        "*.cc",
    ]),
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc:header_system",
    ],
    alwayslink = True,
)
