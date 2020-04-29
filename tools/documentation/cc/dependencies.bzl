load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def documentation_cc_load():
    http_archive(
        name = "doxygen",
        build_file = "//tools/documentation/cc:doxygen.BUILD",
        urls = [
            "http://doxygen.nl/files/doxygen-1.8.18.linux.bin.tar.gz",
        ],
        strip_prefix = "doxygen-1.8.18",
        sha256 = "d589d17c1a5eec7586904a6df4bcdba585ed8e2a0c6afc825bb77e5cb17c275f",
    )
