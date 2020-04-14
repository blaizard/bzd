load("@rules_cc//cc:defs.bzl", "cc_library")

config_setting(
    name = "backend",
    define_values = {
        "log": "stdout",
    },
    visibility = ["//visibility:public"],
)

cc_library(
    name = "stdout",
    srcs = glob([
        "*.cpp",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//cc:bzd_core",
    ],
    alwayslink = True,
)
