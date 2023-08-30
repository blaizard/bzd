load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "payment",
    srcs = [
        "payment.mjs",
        "payment.vue",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
    ],
)
