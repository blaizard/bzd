# Bazel 3rd party rules

load("//tools/bazel_build/rules/third_party:dependencies.bzl", "bazel_rules_load")

bazel_rules_load()

load("//tools/bazel_build/rules/third_party:register.bzl", "bazel_rules_register")

bazel_rules_register()

# Docker

load("//tools/docker_images:register.bzl", "docker_execution_platforms", "docker_images_register")

docker_images_register()

docker_execution_platforms(name = "docker_images")

# Coverage

load("//tools/coverage:dependencies.bzl", "coverage_load")

coverage_load()

# Sanitizer

load("//tools/sanitizer:dependencies.bzl", "sanitizer_load")

sanitizer_load()
