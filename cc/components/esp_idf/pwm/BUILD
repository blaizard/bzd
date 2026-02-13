load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:core",
    ],
)

cc_library(
    name = "pwm",
    srcs = [
        "pwm.cc",
    ],
    hdrs = [
        "pwm.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/core/async",
        "//cc/components/esp_idf:error",
        "//cc/libs/esp_idf",
    ],
)
