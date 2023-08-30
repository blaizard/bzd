load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

string_flag(
    name = "build",
    build_setting_default = "dev",
    values = [
        "dev",
        "prod",
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "dev",
    flag_values = {
        ":build": "dev",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "prod",
    flag_values = {
        ":build": "prod",
    },
    visibility = ["//visibility:public"],
)
