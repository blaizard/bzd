load("@rules_cc//cc:defs.bzl", "cc_library")

config_setting(
    name = "backend",
    define_values = {
        "panic": "trap",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "trap",
    srcs = glob([
        "*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//platform:header_system",
    ],
    alwayslink = True,
)
