"""Loader for debian rules."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_debian_load():
    http_archive(
        name = "debian_repository_rules",
        sha256 = "fc24e3018aa6e331cfa568b6ad28bb2f6851f985270cff2a7364fb4c082cfcda",
        strip_prefix = "debian_repository_rules-3f80031c3ccee36931400323f452f569b8046966",
        type = "zip",
        url = "https://github.com/FaBrand/debian_repository_rules/archive/3f80031c3ccee36931400323f452f569b8046966.zip",
    )
