load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "release",
    srcs = [
        "release.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/db:utils",
        "//nodejs/db/storage:permissions",
        "//nodejs/utils:pathlib",
    ],
)
