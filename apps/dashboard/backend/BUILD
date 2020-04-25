load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.js",
    ]),
    main = "app.js",
    args = [
        "--static", "/tmp/frontend",
    ],
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//apps/dashboard:plugins",
        "//nodejs/core/key_value_store:disk",
        "//nodejs/core:web",
        "//nodejs/core:cache",
        "//nodejs/core:api",
        "//nodejs/core:filesystem",
        "//apps/dashboard:api",
    ],
)
