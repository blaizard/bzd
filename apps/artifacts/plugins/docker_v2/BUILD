load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
    ],
    packages = {
        "http": "",
        "http-proxy": "",
    },
    visibility = [
        "//apps/artifacts/plugins:__pkg__",
    ],
    deps = [
        "//nodejs/core:log",
        "//nodejs/db/storage:docker_v2",
    ],
)

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "frontend.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__pkg__",
    ],
    deps = [
        "//apps/artifacts/plugins:table",
    ],
)
