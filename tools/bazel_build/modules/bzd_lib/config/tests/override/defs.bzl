"""Rule to apply a configuration."""

load("//config:defs.bzl", "make_bzd_config_apply")

bzd_config_apply_test, _test = make_bzd_config_apply(
    configs = [
        Label("//config/tests/override:config_merge"),
    ],
    executable = True,
    test = True,
)
