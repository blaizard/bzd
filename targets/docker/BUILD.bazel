load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_target")
load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

bzd_config(
    name = "config",
    include_workspace_status = ["STABLE_VERSION"],
)

py_binary(
    name = "main",
    srcs = [
        "config.py",
        "main.py",
        "registry.py",
    ],
    data = [
        "registry.yml.btl",
        "rollback.sh",
    ],
    deps = [
        ":docker_compose",
        "//targets/docker/deployment:compose",
        "//targets/docker/deployment:traefik",
        "//targets/docker/transport:ssh",
        "@bzd_bdl//bdl/generators/json/ast",
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/template",
        "@bzd_rules_oci//python/bzd_oci:run",
    ],
)

py_library(
    name = "docker_compose",
    srcs = [
        "docker_compose.py",
    ],
    visibility = [
        "//targets/docker:__subpackages__",
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
    binary = ":main",
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
