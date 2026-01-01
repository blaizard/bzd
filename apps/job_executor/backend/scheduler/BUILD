load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "scheduler",
    srcs = [
        "scheduler.mjs",
    ],
    visibility = [
        "//apps/job_executor/backend:__pkg__",
    ],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/services",
        "//nodejs/utils:run",
    ],
)
