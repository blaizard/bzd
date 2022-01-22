load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

string_flag(
    name = "executor",
    build_setting_default = "default",
    visibility = ["//visibility:public"],
)

# Config setting of known values

config_setting(
    name = "is_sim",
    flag_values = {
        "//tools/bazel_build/settings/executor": "sim",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "is_default",
    flag_values = {
        "//tools/bazel_build/settings/executor": "default",
    },
    visibility = ["//visibility:public"],
)
