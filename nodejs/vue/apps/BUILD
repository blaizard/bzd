load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_config_default(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    visibility = [
        "//visibility:public",
    ],
)

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
        "config.json",
    ],
    data = [
        ":config",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//apps/accounts/authentication:server",
        "//nodejs/core:cache2",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:server",
        "//nodejs/core/http/mock:server",
        "//nodejs/core/rest:server",
        "//nodejs/core/services",
        "//nodejs/core/statistics",
    ],
)

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "application.vue",
        "config.json",
        "frontend.mjs",
    ],
    data = [
        ":config",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//apps/accounts/authentication:client",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/styles/default",
        "//nodejs/vue:authentication",
        "//nodejs/vue:metadata",
        "//nodejs/vue:notification",
        "//nodejs/vue:rest",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/components/menu:entry",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)
