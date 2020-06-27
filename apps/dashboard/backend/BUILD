load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.mjs",
    ]),
    args = [
        "--static",
        "/tmp/bzd/frontend",
        "--data",
        "/tmp/bzd/db",
    ],
    main = "app.mjs",
    packages = {
        "commander": "",
    },
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//apps/dashboard:api",
        "//apps/dashboard/plugins",
        "//nodejs/core:cache",
        "//nodejs/core:filesystem",
        "//nodejs/core:web",
        "//nodejs/core/api:server",
        "//nodejs/core/key_value_store:disk",
    ],
)
