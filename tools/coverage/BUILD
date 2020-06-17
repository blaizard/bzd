load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "report",
    binary = "@lcov//:genhtml",
    command = """
		cd "$BUILD_WORKSPACE_DIRECTORY"
		pwd
        {binary} "$@"
    """,
    visibility = ["//visibility:public"],
)
