load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "screen_recording",
    srcs = [
        "app.mjs",
        "app.vue",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    deps = [
        "//nodejs:vue",
    ],
)
