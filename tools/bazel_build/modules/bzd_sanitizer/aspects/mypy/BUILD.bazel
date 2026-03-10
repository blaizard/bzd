load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "mypy",
    command = """
        if ! {{ error=$({binary} --config-file {config} --show-error-context --pretty --strict "${{@: 2}}"); }} then
            echo "$error"
            exit 1
        fi
        echo "1" > "$1"
    """,
    locations = {
        "@bzd_sanitizer_config//:mypy": "binary",
        "@bzd_sanitizer_config//:mypy_config": "config",
    },
    visibility = ["//visibility:public"],
)
