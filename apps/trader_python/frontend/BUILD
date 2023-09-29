load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = [
        "app.mjs",
        "app.vue",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    visibility = [
        "//apps/trader_python:__pkg__",
    ],
    deps = [
        "//nodejs:vue",
        "//nodejs/styles/default",
        "//nodejs/vue/components/graph",
    ],
)
