load("//tools/bazel.build/rules/third_party/python:register.bzl", "bazel_rules_python_register")
load("//tools/bazel.build/rules/third_party/go:register.bzl", "bazel_rules_go_register")
load("//tools/bazel.build/rules/third_party/gazelle:register.bzl", "bazel_rules_gazelle_register")
load("//tools/bazel.build/rules/third_party/skylib:register.bzl", "bazel_rules_skylib_register")
load("//tools/bazel.build/rules/third_party/docker:register.bzl", "bazel_rules_docker_register")

def bazel_rules_register():
    bazel_rules_python_register()
    bazel_rules_go_register()
    bazel_rules_gazelle_register()
    bazel_rules_skylib_register()
    bazel_rules_docker_register()
