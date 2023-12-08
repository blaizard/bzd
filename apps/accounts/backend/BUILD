load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
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
        "//apps/accounts/backend/pending_actions",
        "//apps/accounts/backend/services",
        "//apps/accounts/backend/tests:test_data",
        "//apps/accounts/backend/users",
        "//nodejs/core/api:server",
        "//nodejs/core/authentication/google:server",
        "//nodejs/core/authentication/session:server",
        "//nodejs/core/http:server",
        "//nodejs/db/key_value_store:memory",
    ],
)
