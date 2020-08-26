load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

_PLUGINS = [
    "fs",
    "docker",
]

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "frontend.mjs",
    ],
    visibility = [
        "//apps/artifacts:__subpackages__",
    ],
    deps = ["//apps/artifacts/plugins/{}:frontend".format(name) for name in _PLUGINS],
)

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
    ],
    visibility = [
        "//apps/artifacts:__subpackages__",
    ],
    deps = ["//apps/artifacts/plugins/{}:backend".format(name) for name in _PLUGINS],
)

bzd_nodejs_library(
    name = "table",
    srcs = [
        "base.vue",
        "table.vue",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
)
