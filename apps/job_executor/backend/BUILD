load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "command",
    srcs = [
        "command.mjs",
    ],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
        "args.mjs",
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
        ":command",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/vue/apps:backend",
        "//nodejs/vue/components/form:backend",
    ],
)
