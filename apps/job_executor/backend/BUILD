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
        "executor.mjs",
        "executor_docker.mjs",
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
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/db/storage:disk",
        "//nodejs/utils:pathlib",
        "//nodejs/utils:run",
        "//nodejs/utils:uid",
        "//nodejs/vue/apps:backend",
        "//nodejs/vue/components/form:backend",
        "//nodejs/vue/components/terminal/backend/docker",
        "//nodejs/vue/components/terminal/backend/local",
    ],
)
