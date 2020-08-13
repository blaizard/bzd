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
        "http": "",
        "http-proxy": "",
    },
    visibility = [
        "//apps/artifacts:__pkg__",
    ],
    deps = [
        "//apps/artifacts:api",
        "//nodejs/core:cache",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:web",
        "//nodejs/core/api:server",
        "//nodejs/db:utils",
        "//nodejs/db/key_value_store:disk",
        "//nodejs/db/storage:disk",
        "//nodejs/db/storage:docker_v2",
    ],
)
