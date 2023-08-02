"""Rules for yarn."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_linux_x86_64_yarn(name = "unused"):
    http_archive(
        name = "yarn",
        build_file = "//toolchains/nodejs/linux_x86_64_yarn:yarn.BUILD",
        urls = [
            "https://github.com/yarnpkg/yarn/releases/download/v1.22.10/yarn-v1.22.10.tar.gz",
        ],
        strip_prefix = "yarn-v1.22.10",
        sha256 = "7e433d4a77e2c79e6a7ae4866782608a8e8bcad3ec6783580577c59538381a6e",
    )

    http_archive(
        name = "node",
        build_file = "//toolchains/nodejs/linux_x86_64_yarn:node.BUILD",
        urls = [
            "https://nodejs.org/dist/v14.4.0/node-v14.4.0-linux-x64.tar.xz",
        ],
        strip_prefix = "node-v14.4.0-linux-x64",
        sha256 = "d65a9a8a547bfe67c6c08dae733a3e5a846700d5377c5f150164cc6bb5f6a039",
    )

    native.register_toolchains("//toolchains/nodejs/linux_x86_64_yarn:toolchain")
