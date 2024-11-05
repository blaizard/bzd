load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "showcase",
    srcs = glob([
        "**/*",
    ]),
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    deps = [
        "//nodejs/styles/default",
        "//nodejs/vue:notification",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/graph",
        "//nodejs/vue/components/layout",
        "//nodejs/vue/components/layout:accordion",
        "//nodejs/vue/components/menu:entry",
        "//nodejs/vue/directives:loading",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/router",
    ],
)
