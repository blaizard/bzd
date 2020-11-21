load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "esp32_xtensa_lx6",
    srcs = [
        "clock.cc",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:headers",
    ],
    alwayslink = True,
)
