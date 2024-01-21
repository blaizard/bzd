load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "make_from_config",
    srcs = [
        "make_from_config.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:validation",
        "//nodejs/payment/mock:payment",
        "//nodejs/payment/stripe:webhook",
    ],
)

bzd_nodejs_library(
    name = "payment",
    srcs = [
        "payment.mjs",
    ],
    visibility = ["//nodejs/payment:__subpackages__"],
)
