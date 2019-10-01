load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "googletest",
    #build_file = "gmock.BUILD",
    urls = [
		"https://github.com/google/googletest/archive/release-1.8.1.zip",
	],
	strip_prefix = "googletest-release-1.8.1",
	sha256 = "927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7"
)

register_toolchains(
    "//tools/bazel/linux_x86_64/gcc",
    "//tools/bazel/linux_x86_64/clang",
)
