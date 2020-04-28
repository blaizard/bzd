load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def bazel_rules_gazelle_load():
    http_archive(
        name = "bazel_gazelle",
        sha256 = "d8c45ee70ec39a57e7a05e5027c32b1576cc7f16d9dd37135b0eddde45cf1b10",
        urls = [
            "https://storage.googleapis.com/bazel-mirror/github.com/bazelbuild/bazel-gazelle/releases/download/v0.20.0/bazel-gazelle-v0.20.0.tar.gz",
            "https://github.com/bazelbuild/bazel-gazelle/releases/download/v0.20.0/bazel-gazelle-v0.20.0.tar.gz",
        ],
    )
