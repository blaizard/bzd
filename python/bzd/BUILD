load("@bzd_lib//config:defs.bzl", "bzd_config")
load("//:defs.bzl", "bzd_py_binary_library")

bzd_config(
    name = "bin_external",
    values = {
        "scp": "scp",
        "ssh": "ssh",
    },
    visibility = ["//bzd:__subpackages__"],
)

bzd_py_binary_library(
    name = "bin",
    executables = {
        "@bzd_rules_oci//:crane": "crane",
    },
    visibility = ["//bzd:__subpackages__"],
)
