load("@debian_repository_rules//:debian.bzl", "debian_archive")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_linux_x86_64_python():
    http_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = [
            "http://data.blaizard.com/file/bzd/toolchains/python3/linux_x86_64_3.8.15.tar.xz",
        ],
        strip_prefix = "linux_x86_64_3.8.15",
        sha256 = "5dd3935f945e695b237e23ec4c981381fedb422eabb8f16118f46de379233402",
    )

    http_archive(
        name = "coveragepy",
        sha256 = "47c5b0ce92eaaa3c66086490319987061538b8eedf02d4ef12857fa4c50f284c",
        build_file = "//toolchains/python/linux_x86_64:coveragepy.BUILD",
        strip_prefix = "coveragepy-4e3cc882db3700912fce5683af37afb11c040768",
        url = "https://github.com/ulfjack/coveragepy/archive/4e3cc882db3700912fce5683af37afb11c040768.zip",
    )

    native.register_toolchains("//toolchains/python/linux_x86_64:toolchain")
