load("@bzd_lib//:defs.bzl", "bzd_executable_test")
load("//apps/artifacts:defs.bzl", "bzd_artifacts_binary")

bzd_artifacts_binary(
    name = "restricted_user",
    configs = {
        "//apps/artifacts/backend:config": ":config.backend.json",
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
