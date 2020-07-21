load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_docker_load():
    http_archive(
        name = "io_bazel_rules_docker",
        sha256 = "4521794f0fba2e20f3bf15846ab5e01d5332e587e9ce81629c7f96c793bb7036",
        strip_prefix = "rules_docker-0.14.4",
        urls = ["https://github.com/bazelbuild/rules_docker/releases/download/v0.14.4/rules_docker-v0.14.4.tar.gz"],
    )
