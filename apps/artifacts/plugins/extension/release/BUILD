load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_config(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    visibility = [
        "//apps/artifacts/plugins/extension/release:__subpackages__",
        "//deployment:__subpackages__",
    ],
)

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
