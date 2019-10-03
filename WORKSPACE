load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "googletest",
    urls = [
		"https://github.com/google/googletest/archive/release-1.8.1.zip",
	],
	strip_prefix = "googletest-release-1.8.1",
	sha256 = "927827c183d01734cc5cfef85e0ff3f5a92ffe6188e0d18e909c5efebf28a0c7"
)

http_archive(
    name = "linux_x86_64_clang_9.0.0",
    build_file = "@//tools/bazel:linux_x86_64/clang/9.0.0.BUILD",
    urls = [
        "http://releases.llvm.org/9.0.0/clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04.tar.xz"
	],
	strip_prefix = "clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04",
	sha256 = "a23b082b30c128c9831dbdd96edad26b43f56624d0ad0ea9edec506f5385038d"
)

register_toolchains(
    "//tools/bazel/linux_x86_64/gcc",
    "@linux_x86_64_clang_9.0.0//:clang",
)
