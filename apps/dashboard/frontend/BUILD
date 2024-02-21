load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
    ]),
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    visibility = [
        "//apps/dashboard:__pkg__",
        "//apps/dashboard/backend:__pkg__",
    ],
    deps = [
        "//apps/dashboard:api",
        "//apps/dashboard/plugins:plugins.frontend",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
        "//nodejs/styles/default",
        "//nodejs/utils:color",
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

bzd_package(
    name = "local",
    args = ["/tmp/bzd"],
    include_metadata = True,
    tags = ["nodejs"],
    deps = {
        ":frontend": "frontend",
    },
)
