load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_config_default(
    name = "config",
    data = [
        ":icon.svg",
    ],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
    ]),
    apis = ["//apps/dashboard:api.v1.json"],
    config_scss = "//nodejs/styles:default.scss",
    data = [
        ":config",
    ],
    main = "app.mjs",
    visibility = [
        "//apps/dashboard:__pkg__",
        "//apps/dashboard/backend:__pkg__",
    ],
    deps = [
        "//apps/dashboard/plugins:plugins.frontend",
        "//nodejs:icons",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
        "//nodejs/styles/default",
        "//nodejs/utils:color",
        "//nodejs/vue/apps:frontend",
        "//nodejs/vue/components/form",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
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
