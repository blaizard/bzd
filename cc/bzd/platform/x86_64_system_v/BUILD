load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "x86_64_system_v",
    srcs = glob([
        "*.S",
        "*.cc",
    ]),
    hdrs = [
        "//cc/bzd/platform:system.h",
    ],
    tags = ["manual"],
    visibility = ["//cc/bzd/platform:__pkg__"],
)
