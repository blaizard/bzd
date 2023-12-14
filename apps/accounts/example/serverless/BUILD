load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_config(
    name = "config",
    srcs = [
        "config.default.json",
    ],
)

bzd_nodejs_web_binary(
    name = "serverless",
    srcs = [
        "app.mjs",
        "app.vue",
        ":config.json",
    ],
    apis = [
        "//apps/accounts/backend/users:api.json",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    deps = [
        "//nodejs/core/authentication/token:client",
        "//nodejs/vue:api",
        "//nodejs/vue:authentication",
    ],
)
