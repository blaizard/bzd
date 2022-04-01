load("@debian_repository_rules//:debian.bzl", "debian_archive")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/python3.8-minimal_3.8.10-0ubuntu1~20.04.4_amd64.deb": "3f9e06aabc6b57ec8cc92187d38947d2dacca5f4778697ecc04b61f8d349d7b4",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-minimal_3.8.10-0ubuntu1~20.04.4_amd64.deb": "1da6d7edc22415a82840cef5f8e08b15f0bc248b93b8d02e14afaf60725ffeda",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-stdlib_3.8.10-0ubuntu1~20.04.4_amd64.deb": "30382d75a11fd8957e831a81f831c66b730bdcbacc4d7637779e2c3f03676b70",
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
