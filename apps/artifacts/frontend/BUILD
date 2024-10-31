load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
    ]) + [
        "//apps/artifacts:config.json",
    ],
    apis = ["//apps/artifacts:api.v1.json"],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    packages = [
        "@nodejs_deps//:vue-async-computed",
    ],
    visibility = [
        "//apps/artifacts:__subpackages__",
    ],
    deps = [
        "//apps/accounts/authentication:client",
        "//apps/artifacts/plugins:frontend",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core/services:frontend",
        "//nodejs/db/storage:permissions",
        "//nodejs/styles/default",
        "//nodejs/vue:authentication",
        "//nodejs/vue:cache",
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
