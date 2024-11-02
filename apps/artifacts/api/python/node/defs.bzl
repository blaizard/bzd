"""Override the configuration of the bootloader."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

node_binary, _ = make_bzd_config_apply(
    target = Label("//apps/artifacts/api/python/node:binary"),
    configs = [
        Label("//apps/artifacts/api:config"),
    ],
    executable = True,
)
