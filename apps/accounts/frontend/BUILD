load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.mjs",
        "**/*.vue",
        "**/*.svg",
    ]),
    config_scss = ":config.scss",
    heads = [
        "<title>bzd accounts</title>",
    ],
    main = "app.mjs",
    packages = {
        "passport": "",
        "passport-google-oidc": "",
    },
    visibility = ["//apps/accounts/backend:__pkg__"],
    deps = [
        "//apps/accounts:api",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core/authentication/google:client",
        "//nodejs/core/authentication/token:client",
        "//nodejs/styles/default",
        "//nodejs/vue:api",
        "//nodejs/vue:authentication",
        "//nodejs/vue:lang",
        "//nodejs/vue:notification",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)
