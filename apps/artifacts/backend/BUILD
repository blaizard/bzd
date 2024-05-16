load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_config(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.mjs",
    ]) + [
        ":config.json",
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
        "//nodejs/core/http:endpoint",
        "//nodejs/core/http:server",
        "//nodejs/core/rest:server",
        "//nodejs/db:utils",
        "//nodejs/db/key_value_store:disk",
    ],
)
