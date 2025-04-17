load("@bzd_bundle//:defs.bzl", "bzd_bundle_binary")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_library(
    name = "factory",
    srcs = [
        "factory.py",
    ],
    data = [
        "stages.json",
    ],
    visibility = [
        "//tools/ci:__subpackages__",
    ],
    deps = [
        "@bzd_python//bzd/template",
    ],
)

py_binary(
    name = "generate",
    srcs = [
        "generate.py",
    ],
    deps = [
        "//tools/ci/gitea:factory",
        "//tools/ci/github:factory",
        "//tools/ci/jenkins:factory",
        "//tools/ci/quality_gate:factory",
        "//tools/ci/travis:factory",
        "@bzd_sanitizer//python/bzd_sanitizer:context",
    ],
)

bzd_bundle_binary(
    name = "bzd_node",
    args = [
        "--uid",
        "ci",
    ],
    executable = "//apps/artifacts/api/python/node:binary",
    tags = ["manual"],
)
