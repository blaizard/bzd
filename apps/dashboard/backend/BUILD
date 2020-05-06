load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.js",
    ]),
    args = [
        "--static",
        "/tmp/frontend",
        "--data",
        "/tmp/db",
    ],
    main = "app.js",
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//apps/dashboard:api",
        "//apps/dashboard/plugins",
        "//nodejs/core:api",
        "//nodejs/core:cache",
        "//nodejs/core:filesystem",
        "//nodejs/core:web",
        "//nodejs/core/key_value_store:disk",
    ],
)
