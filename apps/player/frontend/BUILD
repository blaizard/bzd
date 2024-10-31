load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.mjs",
    ]),
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    packages = [
        "@nodejs_deps//:highlight.js",
    ],
    visibility = [
        "//apps/player:__pkg__",
        "//apps/player/backend:__pkg__",
    ],
    deps = [
        "//apps/player:api",
        "//apps/player/lib:filesystem",
        "//apps/player/lib:scenario",
        "//nodejs:vue",
        "//nodejs/styles/default",
        "//nodejs/vue:rest",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/router",
    ],
)
