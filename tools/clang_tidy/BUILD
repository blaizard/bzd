load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "clang_tidy",
    binary = "@linux_x86_64_clang_14_0_0//:clang_tidy",
    command = """
        ln -s $RUNFILES_DIR/__main__/external external
        ln -s $RUNFILES_DIR/__main__/.clang-tidy .clang-tidy
        {binary} --use-color --quiet "${{@: 2}}" 2> /dev/null
        touch "$1"
    """,
    data = [
        "//:.clang-tidy",
    ],
    visibility = ["//visibility:public"],
)
