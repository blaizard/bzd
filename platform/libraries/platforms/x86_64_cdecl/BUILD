load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "x86_64_cdecl",
    srcs = glob([
        "*.S",
        "*.cpp",
    ]),
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//platform:header_system",
    ],
    alwayslink = True,
)
