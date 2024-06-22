load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_target")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "main",
    srcs = ["main.py"],
    data = [
        "registry.yml.btl",
    ],
    deps = [
        "//targets/docker/deployment:compose",
        "//targets/docker/deployment:traefik",
        "//targets/docker/transport:ssh",
        "@bzd_bdl//bdl/generators/json/ast",
        "@bzd_python//bzd/template",
        "@bzd_python//bzd/http:client",
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
