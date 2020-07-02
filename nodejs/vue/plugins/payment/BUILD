load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "payment",
    srcs = [
        "payment.mjs",
        "payment.vue",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)
