load("@rules_python//python:defs.bzl", "py_binary")
load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_target")

py_binary(
    name = "deployment",
    srcs = ["deployment.py"],
    data = [
        "docker_compose.yml.btl",
    ],
    deps = [
        "@bzd_bdl//bdl/generators/json/ast",
        "@bzd_python//bzd/template",
        "@bzd_python//bzd/utils:ssh",
        "@bzd_rules_oci//python/bzd_oci:run",
    ],
)

bdl_library(
    name = "api",
    srcs = [
        "api.bdl",
    ],
)

bdl_target(
    name = "auto",
    binary = ":deployment",
    composition = [
        "target.bdl",
    ],
    language = "json",
    visibility = ["//visibility:public"],
    deps = [
        ":api",
        "//interfaces:executor",
    ],
)
