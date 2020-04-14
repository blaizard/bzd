load("@rules_cc//cc:defs.bzl", "cc_library")

config_setting(
    name = "backend",
    define_values = {
        "panic": "exit",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "exit",
    srcs = glob([
        "*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//cc:header_system",
    ],
    alwayslink = True,
)
