"""Override the configuration of the accounts application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_accounts_oci, _ = make_bzd_config_apply(
    target = Label("//apps/accounts"),
    configs = [
        Label("//apps/accounts:config"),
        Label("//apps/accounts/backend:config"),
    ],
)
