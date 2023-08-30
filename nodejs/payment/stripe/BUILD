load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "server",
    srcs = [
        "api.json",
        "server.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
    ],
)

bzd_nodejs_library(
    name = "client",
    srcs = [
        "api.json",
        "client.mjs",
        "client.vue",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/vue/components/form",
        "//nodejs/vue/directives:loading",
    ],
)
