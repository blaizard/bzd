load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "fontend",
    srcs = [
        "sonos.svg",
        "source_frontend.mjs",
    ],
    visibility = [
        "//apps/dashboard/plugins:__pkg__",
    ],
)

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "source_backend.mjs",
    ],
    packages = {
        "sonos": "",
    },
    visibility = [
        "//apps/dashboard/plugins:__pkg__",
    ],
)
