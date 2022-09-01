load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def coverage_load():
    http_archive(
        name = "lcov",
        sha256 = "987031ad5528c8a746d4b52b380bc1bffe412de1f2b9c2ba5224995668e3240b",
        build_file = "//tools/coverage:lcov.BUILD",
        strip_prefix = "lcov-1.16",
        url = "https://github.com/linux-test-project/lcov/releases/download/v1.16/lcov-1.16.tar.gz",
    )
