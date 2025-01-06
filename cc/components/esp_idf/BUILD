load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "error",
    hdrs = [
        "error.hh",
    ],
    target_compatible_with = [
        "@bzd_platforms//al:esp_idf",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/core:error",
    ],
)
