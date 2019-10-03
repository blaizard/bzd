cc_toolchain_suite(
    name = "cc_toolchain_suite",
    toolchains = {
        "linux_x86_64|gcc": "//tools/bazel/linux_x86_64/gcc:cc_toolchain",
        "linux_x86_64|clang": "@linux_x86_64_clang_9.0.0//:cc_toolchain",
        "k8|gcc": "//tools/bazel/linux_x86_64/gcc:cc_toolchain",
        "k8|clang": "@linux_x86_64_clang_9.0.0//:cc_toolchain",
        "k8": "//tools/bazel/linux_x86_64/gcc:cc_toolchain",
    },
	visibility = ["//visibility:public"]
)
