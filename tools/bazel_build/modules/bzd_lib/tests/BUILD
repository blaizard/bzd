load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

filegroup(
    name = "echo",
    srcs = [
        "echo.sh",
    ],
)

sh_binary_wrapper(
    name = "wrapper_echo1",
    binary = ":echo",
)

sh_binary_wrapper(
    name = "wrapper_echo2",
    binary = ":wrapper_echo1",
)

sh_binary_wrapper(
    name = "wrapper_echo3",
    binary = ":wrapper_echo2",
)
