load("@bzd_bundle//:defs.bzl", "bzd_bundle")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")
load("//apps/artifacts/api/python/node:defs.bzl", "node_binary")

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

node_binary(
    name = "node",
    configs = {
        "//apps/artifacts/api:config": ":config.artifacts.api.json",
    },
)

bzd_bundle(
    name = "timeit",
    arguments = [
        "--uid",
        "ci",
        "timeit",
    ],
    binary = ":node",
    tags = ["manual"],
)
