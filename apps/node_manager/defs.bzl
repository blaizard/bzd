"""Override the configuration of the node manager application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_node_manager_binary, _ = make_bzd_config_apply(
    target = Label("//apps/node_manager:bundle"),
    configs = [
        Label("//apps/bootloader:config"),
        Label("//apps/artifacts/api:config"),
    ],
    executable = True,
)
