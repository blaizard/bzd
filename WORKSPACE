# Bazel 3rd party rules

load("//tools/bazel_build/rules/third_party:dependencies.bzl", "bazel_rules_load")

bazel_rules_load()

load("//tools/bazel_build/rules/third_party:register.bzl", "bazel_rules_register")

bazel_rules_register()

load("//tools/bazel_build/rules/third_party:post_register.bzl", "bazel_rules_post_register")

bazel_rules_post_register()

# Docker

load("//tools/docker_images:register.bzl", "docker_execution_platforms", "docker_images_register")

docker_images_register()

docker_execution_platforms()

# Toolchains

load("//toolchains:register.bzl", "toolchains_register")

toolchains_register()

# Coverage

load("//tools/coverage:dependencies.bzl", "coverage_load")

coverage_load()

# Sanitizer

load("//tools/sanitizer:dependencies.bzl", "sanitizer_load")

sanitizer_load()
