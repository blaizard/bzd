load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_config_default(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
        "args.mjs",
        "commands.mjs",
        "context.mjs",
        ":config.json",
        "//apps/job_executor:jobs.json",
    ],
    apis = ["//apps/job_executor:api.v1.json"],
    args = [
        "--static",
        "$(rootpath //apps/job_executor/frontend).bundle",
    ],
    data = [
        ":config",
        "//apps/job_executor/frontend",
    ],
    main = "app.mjs",
    visibility = [
        "//apps/job_executor:__pkg__",
    ],
    deps = [
        "//apps/job_executor/backend/executor",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core/services",
        "//nodejs/db/storage:disk",
        "//nodejs/utils:pathlib",
        "//nodejs/utils:uid",
        "//nodejs/vue/apps:backend",
        "//nodejs/vue/components/form:backend",
    ],
)
