"""Override the configuration of the accounts application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_accounts_oci, _ = make_bzd_config_apply(
    target = Label("//apps/accounts"),
    configs = [
        Label("//apps/accounts:config"),
        Label("//apps/accounts/backend:config"),
        Label("//apps/artifacts/api:config"),
    ],
)

bzd_accounts_binary, _binary = make_bzd_config_apply(
    target = Label("//apps/accounts/backend"),
    configs = [
        Label("//apps/accounts:config"),
        Label("//apps/accounts/backend:config"),
    ],
    executable = True,
)
