"""Override the configuration for config_to_env."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_config_to_env_binary, _ = make_bzd_config_apply(
    target = Label("//apps/utils/config_to_env"),
    configs = [
        Label("//apps/utils/config_to_env:config"),
    ],
    executable = True,
)
