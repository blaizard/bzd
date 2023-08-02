"""Loader for buildifier dependencies."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

def buildifier_load():
    http_file(
        name = "buildifier",
        urls = ["https://github.com/bazelbuild/buildtools/releases/download/v6.1.2/buildifier-linux-amd64"],
        sha256 = "51bc947dabb7b14ec6fb1224464fbcf7a7cb138f1a10a3b328f00835f72852ce",
        executable = True,
    )
