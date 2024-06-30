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
    srcs = [
        "app.mjs",
        ":config.json",
        "//apps/accounts:config.json",
    ],
    apis = ["//apps/accounts:api.v1.json"],
    args = [
        "--static",
        "$(rootpath //apps/accounts/frontend).bundle",
    ],
    data = [
        "//apps/accounts/frontend",
    ],
    main = "app.mjs",
    packages = {
        "commander": "",
    },
    visibility = [
        "//apps/accounts:__pkg__",
    ],
    deps = [
        "//apps/accounts/backend/applications",
        "//apps/accounts/backend/email",
        "//apps/accounts/backend/logger/memory",
        "//apps/accounts/backend/tests:test_data",
        "//apps/accounts/backend/users",
        "//nodejs/core/authentication/facebook:server",
        "//nodejs/core/authentication/google:server",
        "//nodejs/core/authentication/session:server",
        "//nodejs/core/http:client",
        "//nodejs/core/http:server",
        "//nodejs/core/rest:server",
        "//nodejs/core/services",
        "//nodejs/db/key_value_store:make_from_config",
        "//nodejs/email:make_from_config",
        "//nodejs/payment:make_from_config",
        "//nodejs/utils:delay",
        "//nodejs/utils:result",
    ],
)
