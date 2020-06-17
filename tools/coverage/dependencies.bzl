load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def coverage_load():
    http_archive(
        name = "lcov",
        sha256 = "14995699187440e0ae4da57fe3a64adc0a3c5cf14feab971f8db38fb7d8f071a",
        build_file = "//tools/coverage:lcov.BUILD",
        strip_prefix = "lcov-1.14",
        url = "https://github.com/linux-test-project/lcov/releases/download/v1.14/lcov-1.14.tar.gz",
    )
