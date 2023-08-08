load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.v1.json",
    ],
    visibility = [
        "//apps/player/backend:__pkg__",
        "//apps/player/frontend:__pkg__",
    ],
)
