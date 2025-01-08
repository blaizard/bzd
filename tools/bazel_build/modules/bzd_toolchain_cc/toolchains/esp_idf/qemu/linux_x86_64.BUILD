load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

# To work in with remote execution, bin/qemu-system-xtensa must be launch, not
# an alias of it and this rule does exactly that.
sh_binary_wrapper(
    name = "qemu-system-xtensa",
    visibility = [
        "//visibility:public",
    ],
    locations = {
        "//:bin/qemu-system-xtensa": "qemu",
        "@bzd_toolchain_cc//toolchains/esp_idf/qemu:executor_qemu": "executor_qemu",
    },
    command = "{executor_qemu} --qemu {qemu} \"$@\"",
    data = glob(["**"]),
)
