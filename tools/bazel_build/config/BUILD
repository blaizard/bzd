load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

package(default_visibility = ["//visibility:public"])

config_setting(
    name = "linux",
    constraint_values = [
        "@platforms//os:linux",
    ],
)

config_setting(
    name = "x86_32",
    constraint_values = [
        "@platforms//cpu:x86_32",
    ],
)

config_setting(
    name = "x86_64",
    constraint_values = [
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "linux_x86_64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "dev",
    define_values = {
        "build": "dev",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "prod",
    define_values = {
        "build": "prod",
    },
    visibility = ["//visibility:public"],
)

# is_test

bool_flag(
    name = "is_test",
    build_setting_default = True,
)

config_setting(
    name = "test",
    flag_values = {
        ":is_test": "True",
    },
)

config_setting(
    name = "binary",
    flag_values = {
        ":is_test": "False",
    },
)
