load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

string_flag(
    name = "build_type",
    build_setting_default = "nodejs",
    values = [
        "nodejs",
        "nodejs_web",
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "nodejs",
    flag_values = {
        ":build_type": "nodejs",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "nodejs_web",
    flag_values = {
        ":build_type": "nodejs_web",
    },
    visibility = ["//visibility:public"],
)
