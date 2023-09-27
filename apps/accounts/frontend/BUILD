load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.mjs",
        "**/*.vue",
        "**/*.svg",
    ]),
    main = "app.mjs",
    visibility = ["//apps/accounts/backend:__pkg__"],
    deps = [
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/styles/default",
        "//nodejs/vue:lang",
        "//nodejs/vue/components/form",
        "//nodejs/vue/router",
    ],
)
