load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
        "args.mjs",
        "commands.mjs",
        "//apps/job_executor:jobs.json",
    ],
    apis = ["//apps/job_executor:api.v1.json"],
    args = [
        "--static",
        "$(rootpath //apps/job_executor/frontend).bundle",
    ],
    data = [
        "//apps/job_executor/frontend",
    ],
    main = "app.mjs",
    visibility = [
        "//apps/job_executor:__pkg__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/vue/apps:backend",
        "//nodejs/vue/components/form:backend",
        "//nodejs/vue/components/terminal/backend:command",
    ],
)
