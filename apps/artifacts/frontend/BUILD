load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
    ]),
    apis = ["//apps/artifacts:api.v1.json"],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    packages = [
        "@nodejs_deps//:vue-async-computed",
    ],
    visibility = [
        "//apps/artifacts:__subpackages__",
    ],
    deps = [
        "//apps/artifacts/common:utils",
        "//apps/artifacts/plugins:frontend",
        "//nodejs/db/storage:permissions",
        "//nodejs/vue:cache",
        "//nodejs/vue/apps:frontend",
        "//nodejs/vue/components/form",
    ],
)
