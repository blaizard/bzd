load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "logger.vue",
    ],
    apis = ["//nodejs/vue/components/logger:api.json"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout:component",
    ],
)
