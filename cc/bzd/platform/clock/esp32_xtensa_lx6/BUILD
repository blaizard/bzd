load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "esp32_xtensa_lx6",
    srcs = [
        "clock.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:headers",
    ],
    tags = ["manual"],
    alwayslink = True,
)
