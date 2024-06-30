load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.mjs",
        "**/*.vue",
        "**/*.svg",
    ]) + ["//apps/accounts:config.json"],
    apis = ["//apps/accounts:api.v1.json"],
    config_scss = ":config.scss",
    heads = [
        "<title>Accounts</title>",
        "<meta name=\"referrer\" content=\"no-referrer-when-downgrade\" />",
    ],
    main = "app.mjs",
    visibility = [
        "//apps/accounts:__pkg__",
        "//apps/accounts/backend:__pkg__",
    ],
    deps = [
        "//apps/accounts/backend/applications:frontend",
        "//apps/accounts/backend/logger/memory:frontend",
        "//apps/accounts/backend/users:frontend",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core/authentication/facebook:client",
        "//nodejs/core/authentication/google:client",
        "//nodejs/core/authentication/token:client",
        "//nodejs/core/services:frontend",
        "//nodejs/styles/default",
        "//nodejs/vue:authentication",
        "//nodejs/vue:lang",
        "//nodejs/vue:notification",
        "//nodejs/vue:rest",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/components/menu:entry",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)
