"""Loader for CC rules."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_cc_load():
    http_archive(
        name = "rules_cc",
        urls = ["https://github.com/bazelbuild/rules_cc/releases/download/0.0.4/rules_cc-0.0.4.tar.gz"],
        sha256 = "af6cc82d87db94585bceeda2561cb8a9d55ad435318ccb4ddfee18a43580fb5d",
        strip_prefix = "rules_cc-0.0.4",
    )
