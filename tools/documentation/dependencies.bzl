load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_documentation_dependencies():
    http_archive(
        name = "doxygen",
        urls = [
            "http://doxygen.nl/files/doxygen-1.8.16.linux.bin.tar.gz",
        ],
        strip_prefix = "doxygen-1.8.16",
        sha256 = "610697eba6104110389d371e6908ed35b4b00f9e650c969a213dc0020b067a55",
        build_file = "//tools/documentation:doxygen.BUILD",
    )
