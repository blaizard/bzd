load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "static",
    srcs = [
        "app.mjs",
        "app.vue",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    deps = [
        "//nodejs/core/authentication/token:client",
        "//nodejs/vue:api",
        "//nodejs/vue:authentication",
    ],
)
