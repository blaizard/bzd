load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "serverless",
    srcs = [
        "app.mjs",
        "app.vue",
        "//apps/accounts/example:config.json",
    ],
    apis = [
        "//apps/accounts/backend/users:api.json",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    deps = [
        "//apps/accounts/authentication:client",
        "//nodejs/vue:authentication",
        "//nodejs/vue:rest",
    ],
)
