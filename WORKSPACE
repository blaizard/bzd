load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Bazel 3rd party rules

load("//tools/bazel.build/rules/third_party:dependencies.bzl", "bazel_rules_load")

bazel_rules_load()

load("//tools/bazel.build/rules/third_party:register.bzl", "bazel_rules_register")

bazel_rules_register()

# Docker

load("//tools/docker:register.bzl", "docker_images_register")

docker_images_register()

# Toolchains

load("//toolchains:register.bzl", "toolchains_register")

toolchains_register()

# Buildifier

load("//tools/buildifier:dependencies.bzl", "load_buildifier_dependencies")

load_buildifier_dependencies()

# Documentation

load("//tools/documentation:dependencies.bzl", "load_documentation_dependencies")

load_documentation_dependencies()

# Sanitizer

load("//tools/sanitizer:dependencies.bzl", "load_sanitizer_dependencies")

load_sanitizer_dependencies()
