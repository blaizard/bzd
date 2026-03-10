load("@bzd_lib//config:defs.bzl", "bzd_config_default")

bzd_config_default(
    name = "config",
    srcs = [
        "config.default.json",
    ],
    visibility = [
        "//apps/artifacts/api:__subpackages__",
        "//deployment:__subpackages__",
        "//tools/ci:__subpackages__",
    ],
)
