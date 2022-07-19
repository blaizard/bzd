load("@debian_repository_rules//:debian.bzl", "debian_archive")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/python3.8-minimal_3.8.10-0ubuntu1~20.04.5_amd64.deb": "348bd75383de9bd3324ac90a4c6e8d4466914a9368b72b6dfc508bb617ec1ac7",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-minimal_3.8.10-0ubuntu1~20.04.5_amd64.deb": "199d40e0703d9b307bbbc5a79b24ebf45ce60e0e7e81655fbdfc4448b2654aa9",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-stdlib_3.8.10-0ubuntu1~20.04.5_amd64.deb": "e50798a7dd8dc09e633489d53937e1d603a2c4fa24f76b51b2967b6ece882f30",
            "http://archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-lib2to3_3.8.10-0ubuntu1~20.04_all.deb": "d25b7a114ab08e6f318d96ae8d71c3c24c3b9eaceb84cb1abf476979875521ab",
        },
    )

    http_archive(
        name = "coveragepy",
        sha256 = "47c5b0ce92eaaa3c66086490319987061538b8eedf02d4ef12857fa4c50f284c",
        build_file = "//toolchains/python/linux_x86_64:coveragepy.BUILD",
        strip_prefix = "coveragepy-4e3cc882db3700912fce5683af37afb11c040768",
        url = "https://github.com/ulfjack/coveragepy/archive/4e3cc882db3700912fce5683af37afb11c040768.zip",
    )

    native.register_toolchains("//toolchains/python/linux_x86_64:toolchain")
