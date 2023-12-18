load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
    name = "frontend",
    srcs = [
        "frontend.mjs",
        "frontend.vue",
        "//apps/accounts/example:config.json",
    ],
    apis = [
        "//apps/accounts/backend/users:api.json",
    ],
    config_scss = "//nodejs/styles:default.scss",
    main = "frontend.mjs",
    deps = [
        "//apps/accounts/authentication:client",
        "//nodejs/vue:api",
        "//nodejs/vue:authentication",
    ],
)

bzd_nodejs_binary(
    name = "server",
    srcs = [
        "backend.mjs",
        "//apps/accounts/example:config.json",
    ],
    args = [
        "$(rootpath :frontend).bundle",
    ],
    data = [
        ":frontend",
    ],
    main = "backend.mjs",
    deps = [
        "//nodejs/core/api:server",
        "//nodejs/core/authentication/session:server_proxy",
        "//nodejs/core/http:server",
    ],
)
