"""Override the configuration of the dashboard application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_dashboard_oci, _oci = make_bzd_config_apply(
    target = Label("//apps/dashboard"),
    configs = [
        Label("//apps/dashboard/backend:config"),
    ],
)

bzd_dashboard_binary, _binary = make_bzd_config_apply(
    target = Label("//apps/dashboard/backend"),
    configs = [
        Label("//apps/dashboard/backend:config"),
    ],
    executable = True,
)
