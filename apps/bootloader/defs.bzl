"""Override the configuration of the bootloader."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_bootloader_binary, _ = make_bzd_config_apply(
    target = Label("//apps/bootloader"),
    configs = [
        Label("//apps/bootloader:config"),
        Label("//apps/artifacts/api:config"),
    ],
    executable = True,
)
