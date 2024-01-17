load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_config(
    name = "config",
    srcs = [
        "config.default.json",
    ],
)

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
        ":config.json",
        "//apps/accounts:config.json",
        "//apps/accounts/backend/emails:reset_password.mjs",
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
        "//apps/accounts/backend/logger/memory",
        "//apps/accounts/backend/tests:test_data",
        "//apps/accounts/backend/users",
        "//nodejs/core:template",
        "//nodejs/core/api:server",
        "//nodejs/core/authentication/google:server",
        "//nodejs/core/authentication/session:server",
        "//nodejs/core/http:server",
        "//nodejs/db/key_value_store:make_from_config",
        "//nodejs/email:nodemailer",
        "//nodejs/utils:result",
    ],
)
