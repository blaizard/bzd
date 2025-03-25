load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary", "bzd_nodejs_library")

bzd_config_default(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    visibility = [
        "//deployment:__subpackages__",
    ],
)

bzd_nodejs_library(
    name = "plugin",
    srcs = [
        "plugin.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/db/storage",
    ],
)

bzd_nodejs_library(
    name = "plugin_tester",
    srcs = [
        "endpoints_factory.mjs",
        "plugin_tester.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:router",
        "//nodejs/core/http/mock:server_context",
        "//nodejs/core/services",
    ],
)

bzd_nodejs_binary(
    name = "backend",
    srcs = [
        "app.mjs",
        "endpoints_factory.mjs",
        ":config.json",
        "//apps/artifacts:config.json",
    ],
    apis = ["//apps/artifacts:api.v1.json"],
    args = [
        "--static",
        "$(rootpath //apps/artifacts/frontend).bundle",
    ],
    data = [
        ":config",
        "//apps/artifacts/frontend",
    ],
    main = "app.mjs",
    packages = [
        "@nodejs_deps//:commander",
    ],
    visibility = [
        "//apps/artifacts:__pkg__",
    ],
    deps = [
        ":plugin",
        "//apps/accounts/authentication:server",
        "//apps/artifacts/plugins:backend",
        "//nodejs/core:cache",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core/http:endpoint",
        "//nodejs/core/http:server",
        "//nodejs/core/rest:server",
        "//nodejs/core/services",
        "//nodejs/db:utils",
        "//nodejs/db/key_value_store:disk",
    ],
)
