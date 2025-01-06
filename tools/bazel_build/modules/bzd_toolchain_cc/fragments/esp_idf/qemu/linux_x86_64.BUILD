load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

# To work in with remote execution, bin/qemu-system-xtensa must be launch, not
# an alias of it and this rule does exactly that.
sh_binary_wrapper(
    name = "qemu-system-xtensa",
    binary = "//:bin/qemu-system-xtensa",
    visibility = [
        "//visibility:public",
    ],
    data = glob(["**"]),
)
