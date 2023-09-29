load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "nodejs_web",
    srcs = [
        "app.mjs",
        "app.vue",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    packages = {
        "@vue/composition-api": "latest",
    },
    deps = [
        "//nodejs:vue",
    ],
)
