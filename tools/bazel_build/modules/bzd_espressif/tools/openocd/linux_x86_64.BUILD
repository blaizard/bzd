load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

sh_binary_wrapper(
    name = "openocd",
    visibility = [
        "//visibility:public",
    ],
    locations = {
        "//:bin/openocd": "openocd",
        "//:share/openocd/scripts": "scripts",
    },
    command = "{openocd} -s {scripts} \"$@\"",
    data = glob(["**"]),
)
