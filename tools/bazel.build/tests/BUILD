load("//tools/bazel.build:defs.bzl", "sh_binary_wrapper")

filegroup(
	name = "echo",
	srcs = [
		"echo.sh"
	]
)

sh_binary_wrapper(
    name = "wrapper_echo",
    binary = ":echo",
)
