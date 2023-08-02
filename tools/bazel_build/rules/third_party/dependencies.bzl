"""Loader fo all bazel rules."""

load("//tools/bazel_build/rules/third_party/cc:dependencies.bzl", "bazel_rules_cc_load")
load("//tools/bazel_build/rules/third_party/debian:dependencies.bzl", "bazel_rules_debian_load")
load("//tools/bazel_build/rules/third_party/docker:dependencies.bzl", "bazel_rules_docker_load")
load("//tools/bazel_build/rules/third_party/gazelle:dependencies.bzl", "bazel_rules_gazelle_load")
load("//tools/bazel_build/rules/third_party/go:dependencies.bzl", "bazel_rules_go_load")
load("//tools/bazel_build/rules/third_party/skylib:dependencies.bzl", "bazel_rules_skylib_load")
load("//tools/bazel_build/rules/third_party/stardoc:dependencies.bzl", "bazel_rules_stardoc_load")

def bazel_rules_load():
    """Load all rules."""

    bazel_rules_cc_load()
    bazel_rules_debian_load()
    bazel_rules_go_load()
    bazel_rules_gazelle_load()
    bazel_rules_skylib_load()
    bazel_rules_stardoc_load()
    bazel_rules_docker_load()
