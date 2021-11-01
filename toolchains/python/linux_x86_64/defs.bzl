load("@debian_repository_rules//:debian.bzl", "debian_archive")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/python3.8-minimal_3.8.10-0ubuntu1~20.04.1_amd64.deb": "5a385a535f10e3c8cddc858f91944e99daeeb5c6f20cbcc45d8af1bf51da026f",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-minimal_3.8.10-0ubuntu1~20.04.1_amd64.deb": "dadf7f06cf188717b8909df8c5e871cc3e4acb4b33a49be8b2057b5c8e8a03ce",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-stdlib_3.8.10-0ubuntu1~20.04.1_amd64.deb": "f8faed4c79d86839702de61196793d9aeae728d9322df25f95da1f58d87d2214",
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
