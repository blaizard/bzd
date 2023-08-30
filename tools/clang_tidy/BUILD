load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "clang_tidy",
    binary = "@clang//:clang_tidy",
    command = """
        ln -s $RUNFILES_DIR external
        ln -s $RUNFILES_DIR/__main__/.clang-tidy .clang-tidy
        if ! {{ error=$({binary} --use-color --quiet "${{@: 2}}" 2>&1 >&3); }} 3>&1; then
            echo "$error"
            exit 1
        fi
        touch "$1"
    """,
    data = [
        "//:.clang-tidy",
    ],
    visibility = ["//visibility:public"],
)
