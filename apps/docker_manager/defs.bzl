"""Override the configuration of the docker manager application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_docker_manager_oci, _ = make_bzd_config_apply(
    target = Label("//apps/docker_manager:image"),
    configs = [
        Label("//apps/artifacts/api:config"),
    ],
)
