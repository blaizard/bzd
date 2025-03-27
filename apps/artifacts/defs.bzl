"""Override the configuration of the artifacts application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_artifacts_oci, _ = make_bzd_config_apply(
    target = Label("//apps/artifacts"),
    configs = [
        Label("//apps/artifacts:config"),
        Label("//apps/artifacts/backend:config"),
    ],
)

bzd_artifacts_binary, _binary = make_bzd_config_apply(
    target = Label("//apps/artifacts/backend"),
    configs = [
        Label("//apps/artifacts:config"),
        Label("//apps/artifacts/backend:config"),
    ],
    executable = True,
)
