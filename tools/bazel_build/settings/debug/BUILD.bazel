load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "debug",
    build_setting_default = False,
    visibility = ["//visibility:public"],
)

config_setting(
    name = "no",
    flag_values = {
        ":debug": "False",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "yes",
    flag_values = {
        ":debug": "True",
    },
    visibility = ["//visibility:public"],
)
