load("//tools/bazel_build/rules/third_party/go:register.bzl", "bazel_rules_go_register")
load("//tools/bazel_build/rules/third_party/gazelle:register.bzl", "bazel_rules_gazelle_register")
load("//tools/bazel_build/rules/third_party/skylib:register.bzl", "bazel_rules_skylib_register")
load("//tools/bazel_build/rules/third_party/stardoc:register.bzl", "bazel_rules_stardoc_register")
load("//tools/bazel_build/rules/third_party/docker:register.bzl", "bazel_rules_docker_register")

def bazel_rules_register():
    bazel_rules_go_register()
    bazel_rules_gazelle_register()
    bazel_rules_skylib_register()
    bazel_rules_stardoc_register()
    bazel_rules_docker_register()
