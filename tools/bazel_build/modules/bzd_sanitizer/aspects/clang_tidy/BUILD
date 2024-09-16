load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "clang_tidy",
    binary = "@bzd_sanitizer_config//:clang_tidy",
    command = """
        if ! {{ error=$({binary} --use-color --quiet "${{@: 2}}" 2>&1 >&3); }} 3>&1; then
            echo "$error"
            exit 1
        fi
        echo "1" > "$1"
    """,
    visibility = ["//visibility:public"],
)
