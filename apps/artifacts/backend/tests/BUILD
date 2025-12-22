load("@bzd_lib//:defs.bzl", "bzd_executable_test")
load("@bzd_lib//config:defs.bzl", "bzd_config")
load("//apps/artifacts:defs.bzl", "bzd_artifacts_binary")

bzd_config(
    name = "config_backend",
    srcs = [
        "config.backend.json",
    ],
    base = "//apps/artifacts/backend:config.base",
)

bzd_artifacts_binary(
    name = "restricted_user",
    configs = {
        "//apps/artifacts/backend:config": ":config_backend",
        "//nodejs/vue/apps:config": ":config.apps.json",
    },
)

bzd_executable_test(
    name = "backend_tests",
    args = [
        "--test",
    ],
    executable = ":restricted_user",
    expected_returncode = "0",
)
