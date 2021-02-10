load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def coverage_load():
    http_archive(
        name = "lcov",
        sha256 = "c1cda2fa33bec9aa2c2c73c87226cfe97de0831887176b45ee523c5e30f8053a",
        build_file = "//tools/coverage:lcov.BUILD",
        strip_prefix = "lcov-1.15",
        url = "https://github.com/linux-test-project/lcov/releases/download/v1.15/lcov-1.15.tar.gz",
    )
