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
        "//apps/dashboard:config.json",
    ],
    apis = ["//apps/dashboard:api.v1.json"],
    args = [
        "--static",
        "$(rootpath //apps/dashboard/frontend).bundle",
    ],
    data = [
        "//apps/dashboard/frontend",
    ],
    main = "app.mjs",
    packages = [
        "@nodejs_deps//:commander",
    ],
    visibility = [
        "//apps/dashboard:__pkg__",
    ],
    deps = [
        "//apps/dashboard/plugins:plugins.backend",
        "//nodejs/core:cache",
        "//nodejs/core:filesystem",
        "//nodejs/core/http:client",
        "//nodejs/utils:uid",
        "//nodejs/vue/apps:backend",
    ],
)

bzd_executable_test(
    name = "tests",
    args = [
        "--test",
    ],
    executable = ":backend",
    expected_returncode = "0",
)
