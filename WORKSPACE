load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "googletest",
    urls = [
		"https://github.com/google/googletest/archive/release-1.8.1.zip",
	],
	strip_prefix = "googletest-release-1.8.1",
	sha256 = "927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7"
)

load("//tools/bazel:toolchains/linux_x86_64_clang_9_0_0.bzl", "load_linux_x86_64_clang_9_0_0")

load_linux_x86_64_clang_9_0_0()

register_toolchains(
    "//tools/bazel/linux_x86_64/gcc",
    "@linux_x86_64_clang//:toolchain",
)
