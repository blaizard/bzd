load("//tools/bazel_build/rules/third_party/docker:post_register.bzl", "bazel_rules_docker_post_register")
load("//tools/bazel_build/rules/third_party/python:post_register.bzl", "bazel_rules_python_post_register")

def bazel_rules_post_register():
    bazel_rules_docker_post_register()
    bazel_rules_python_post_register()
