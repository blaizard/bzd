load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def documentation_bazel_load():
    http_archive(
        name = "io_bazel_stardoc",
        urls = [
            "https://github.com/bazelbuild/stardoc/archive/0.4.0.zip",
        ],
        strip_prefix = "stardoc-0.4.0",
        sha256 = "36b8d6c2260068b9ff82faea2f7add164bf3436eac9ba3ec14809f335346d66a",
    )
