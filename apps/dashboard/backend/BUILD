load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_package(
    name = "package",
    deps = {
        ":backend": "hello",
    },
)

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
    ],
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
        "//apps/dashboard/plugins:plugins.backend",
        "//nodejs/core:cache",
        "//nodejs/core:filesystem",
        "//nodejs/core/api:server",
        "//nodejs/core/http:server",
        "//nodejs/db/key_value_store:disk",
    ],
)
