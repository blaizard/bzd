load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
        "**/*.mp3",
        "**/*.wav",
    ]),
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    packages = {
        "highlight.js": "",
    },
    visibility = [
        "//apps/player:__pkg__",
    ],
    deps = [
        "//apps/player:api",
        "//apps/player/lib:filesystem",
        "//apps/player/lib:scenario",
        "//nodejs:vue",
        "//nodejs/styles/default",
        "//nodejs/vue:api",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:component",
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
