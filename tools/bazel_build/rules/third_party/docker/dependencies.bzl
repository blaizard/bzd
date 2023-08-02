"""Loader for docker rules."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_docker_load():
    http_archive(
        name = "io_bazel_rules_docker",
        sha256 = "59536e6ae64359b716ba9c46c39183403b01eabfbd57578e84398b4829ca499a",
        strip_prefix = "rules_docker-0.22.0",
        urls = ["https://github.com/bazelbuild/rules_docker/releases/download/v0.22.0/rules_docker-v0.22.0.tar.gz"],
    )
