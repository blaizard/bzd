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
    packages = {
        "vue-async-computed": "",
    },
    visibility = [
        "//apps/artifacts:__pkg__",
    ],
    deps = [
        "//apps/artifacts:api",
        "//apps/artifacts/plugins:frontend",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core/authentication/token:client",
        "//nodejs/db/storage:permissions",
        "//nodejs/styles/default",
        "//nodejs/vue:api",
        "//nodejs/vue:authentication",
        "//nodejs/vue:cache",
        "//nodejs/vue:notification",
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
