load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "bootstrap",
    build_setting_default = False,
)

config_setting(
    name = "is_bootstrap",
    flag_values = {
        ":bootstrap": "True",
    },
)
