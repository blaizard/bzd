load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "rest",
    srcs = [
        "rest.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core/rest:client",
    ],
)

bzd_nodejs_library(
    name = "authentication",
    srcs = [
        "authentication.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "cache",
    srcs = [
        "cache.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:cache",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "lang",
    srcs = [
        "lang.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:localstorage",
    ],
)

bzd_nodejs_library(
    name = "metadata",
    srcs = [
        "metadata.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:exception",
        "//nodejs/core:validation",
    ],
)

bzd_nodejs_library(
    name = "notification",
    srcs = [
        "notification.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "payment",
    srcs = [
        "payment.vue",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
    ],
)
