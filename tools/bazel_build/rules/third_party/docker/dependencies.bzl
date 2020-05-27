load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_docker_load():
    http_archive(
        name = "io_bazel_rules_docker",
        sha256 = "3efbd23e195727a67f87b2a04fb4388cc7a11a0c0c2cf33eec225fb8ffbb27ea",
        strip_prefix = "rules_docker-0.14.2",
        urls = ["https://github.com/bazelbuild/rules_docker/releases/download/v0.14.2/rules_docker-v0.14.2.tar.gz"],
    )
