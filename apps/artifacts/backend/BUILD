load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.mjs",
    ]) + [
        "//apps/artifacts:config.json",
    ],
    apis = ["//apps/artifacts:api.v1.json"],
    args = [
        "--static",
        "$(rootpath //apps/artifacts/frontend).bundle",
        "--data",
        "/tmp/bzd/db",
    ],
    data = [
        "//apps/artifacts/frontend",
    ],
    main = "app.mjs",
    packages = {
        "commander": "",
    },
    visibility = [
        "//apps/artifacts:__pkg__",
    ],
    deps = [
        "//apps/artifacts/plugins:backend",
        "//nodejs/core:cache",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core/api:server",
        "//nodejs/core/authentication/session:server_proxy",
        "//nodejs/core/http:endpoint",
        "//nodejs/core/http:server",
        "//nodejs/db:utils",
        "//nodejs/db/key_value_store:disk",
    ],
)
