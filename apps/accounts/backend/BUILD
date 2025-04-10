load("@bzd_lib//:executable_test.bzl", "bzd_executable_test")
load("@bzd_lib//config:defs.bzl", "bzd_config_default")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_config_default(
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
    packages = [
        "@nodejs_deps//:commander",
    ],
    visibility = [
        "//apps/accounts:__pkg__",
    ],
    deps = [
        "//apps/accounts/backend/applications",
        "//apps/accounts/backend/email",
        "//apps/accounts/backend/logger/memory",
        "//apps/accounts/backend/test_data",
        "//apps/accounts/backend/users",
        "//nodejs/core:filesystem",
        "//nodejs/core/authentication/facebook:server",
        "//nodejs/core/authentication/google:server",
        "//nodejs/core/authentication/session:server",
        "//nodejs/core/http:client",
        "//nodejs/core/http:server",
        "//nodejs/core/http/mock:server",
        "//nodejs/core/rest:server",
        "//nodejs/core/services",
        "//nodejs/db/key_value_store:make_from_config",
        "//nodejs/email:make_from_config",
        "//nodejs/payment:make_from_config",
        "//nodejs/utils:delay",
        "//nodejs/utils:result",
    ],
)

bzd_executable_test(
    name = "tests",
    args = [
        "--test",
        "--test-data",
    ],
    executable = ":backend",
    expected_returncode = "0",
)
