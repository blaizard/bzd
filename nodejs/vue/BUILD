load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core/api:client",
    ],
)

bzd_nodejs_library(
    name = "authentication",
    srcs = [
        "authentication.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "cache",
    srcs = [
        "cache.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:cache",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "lang",
    srcs = [
        "lang.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:localstorage",
    ],
)

bzd_nodejs_library(
    name = "notification",
    srcs = [
        "notification.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "payment",
    srcs = [
        "payment.vue",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)
