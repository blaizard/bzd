load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
    ],
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
    deps = [
        "//apps/accounts:api",
        "//nodejs/core/api:server",
        "//nodejs/core/authentication/token:server",
        "//nodejs/core/http:server",
        "//nodejs/db/key_value_store:disk",
    ],
)
