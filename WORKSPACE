# Bazel 3rd party rules

load("//tools/bazel_build/rules/third_party:dependencies.bzl", "bazel_rules_load")

bazel_rules_load()

load("//tools/bazel_build/rules/third_party:register.bzl", "bazel_rules_register")

bazel_rules_register()

# Docker

load("//tools/docker:register.bzl", "docker_images_register")

docker_images_register()

# Toolchains

load("//toolchains:register.bzl", "toolchains_register")

toolchains_register()

# Buildifier

load("//tools/buildifier:dependencies.bzl", "buildifier_load")

buildifier_load()

# Documentation

load("//tools/documentation:dependencies.bzl", "documentation_load")

documentation_load()

# Sanitizer

load("//tools/sanitizer:dependencies.bzl", "sanitizer_load")

sanitizer_load()

# Install python pip repository

load("@python_deps//:requirements.bzl", "pip_install")

pip_install()
