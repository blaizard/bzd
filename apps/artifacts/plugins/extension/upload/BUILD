load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "upload",
    srcs = [
        "upload.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/core:format",
        "//nodejs/db:utils",
        "//nodejs/db/storage:permissions",
        "//nodejs/utils:pathlib",
    ],
)
