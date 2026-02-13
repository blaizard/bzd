load("@bzd_bdl//:defs.bzl", "bdl_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:timer",
    ],
)

cc_library(
    name = "gptimer",
    srcs = [
        "gptimer.cc",
    ],
    hdrs = [
        "gptimer.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/core:print",
        "//cc/bzd/core/async",
        "//cc/components/esp_idf:error",
        "//cc/libs/esp_idf",
        "//cc/libs/timer:timer_isr",
    ],
)
