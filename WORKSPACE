load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "googletest",
    sha256 = "927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7",
    strip_prefix = "googletest-release-1.8.1",
    urls = [
        "https://github.com/google/googletest/archive/release-1.8.1.zip",
    ],
)

# Toolchains

load("//tools/bazel.build:toolchains/linux_x86_64_clang.bzl", "load_linux_x86_64_clang_9_0_0")

load_linux_x86_64_clang_9_0_0(name = "linux_x86_64_clang")

register_toolchains(
    "@linux_x86_64_clang//:toolchain",
)

# Buildifier

load("//tools/buildifier:dependencies.bzl", "load_buildifier_dependencies")

load_buildifier_dependencies()

load("//tools/buildifier:buildifier.bzl", "load_buildifier")

load_buildifier()

# Documentation

load("//tools/documentation:dependencies.bzl", "load_documentation_dependencies")

load_documentation_dependencies()
