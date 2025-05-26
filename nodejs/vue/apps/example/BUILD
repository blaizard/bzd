load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary", "bzd_nodejs_web_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "backend.mjs",
    ],
    args = [
        "--static",
        "$(rootpath :frontend).bundle",
    ],
    data = [
        ":frontend",
    ],
    main = "backend.mjs",
    deps = [
        "//nodejs/vue/apps:backend",
    ],
)

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = [
        "frontend.mjs",
        "frontend.vue",
        "icon.svg",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "frontend.mjs",
    deps = [
        "//nodejs/vue/apps:frontend",
    ],
)
