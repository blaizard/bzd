load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:io",
    ],
)

cc_library(
    name = "digital",
    hdrs = [
        "digital.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/core/async",
        "@bzd_espressif//hal/cc",
    ],
)
