load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

def buildifier_load():
    http_file(
        name = "buildifier",
        urls = ["https://github.com/bazelbuild/buildtools/releases/download/2.2.1/buildifier"],
        sha256 = "731a6a9bf8fca8a00a165cd5b3fbac9907a7cf422ec9c2f206b0a76c0a7e3d62",
        executable = True,
    )
