load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "executor",
    srcs = [
        "executor.mjs",
        "executor_docker.mjs",
        "executor_readonly.mjs",
        "executor_shell.mjs",
    ],
    visibility = [
        "//apps/job_executor/backend:__pkg__",
    ],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/utils:run",
        "//nodejs/vue/components/terminal/backend/docker",
        "//nodejs/vue/components/terminal/backend/local",
    ],
)
