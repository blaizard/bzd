load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = glob([
        "**/*.vue",
        "**/*.svg",
        "**/*.mjs",
    ]) + [
        "//apps/job_executor:jobs.json",
    ],
    apis = ["//apps/job_executor:api.v1.json"],
    config_scss = "//nodejs/styles:default.scss",
    main = "app.mjs",
    visibility = [
        "//apps/job_executor:__pkg__",
        "//apps/job_executor/backend:__pkg__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/db/storage:permissions",
        "//nodejs/utils:to_string",
        "//nodejs/vue/apps:frontend",
        "//nodejs/vue/components/file_explorer",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/components/terminal/frontend:websocket",
        "//nodejs/vue/directives:loading",
    ],
)
