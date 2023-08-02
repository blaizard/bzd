"""Loader."""

load("//tools/bazel_build/rules/third_party/docker:register.bzl", "bazel_rules_docker_register")
load("//tools/bazel_build/rules/third_party/gazelle:register.bzl", "bazel_rules_gazelle_register")
load("//tools/bazel_build/rules/third_party/go:register.bzl", "bazel_rules_go_register")
load("//tools/bazel_build/rules/third_party/skylib:register.bzl", "bazel_rules_skylib_register")
load("//tools/bazel_build/rules/third_party/stardoc:register.bzl", "bazel_rules_stardoc_register")

def bazel_rules_register():
    """Register all rules."""

    bazel_rules_go_register()
    bazel_rules_gazelle_register()
    bazel_rules_skylib_register()
    bazel_rules_stardoc_register()
    bazel_rules_docker_register()
