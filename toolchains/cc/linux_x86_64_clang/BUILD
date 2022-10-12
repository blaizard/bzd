load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")
load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper")

bzd_cc_test(
    name = "test",
    srcs = ["clang_test.cc"],
    target_compatible_with = [
        "@linux_x86_64_clang//:compiler",
    ],
    deps = [
        "//cc/bzd/test",
    ],
)

alias(
    name = "clang_format",
    actual = "@linux_x86_64_clang_14_0_6//:clang_format",
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "clang_tidy",
    binary = "@linux_x86_64_clang_14_0_6//:clang_tidy",
    command = """
export LLVM_SYMBOLIZER_PATH="$(location @linux_x86_64_clang_14_0_6//:llvm_symbolizer)"
{binary} $@
    """,
    data = [
        "@linux_x86_64_clang_14_0_6//:llvm_symbolizer",
    ],
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "executor_host",
    binary = "//tools/bazel_build/toolchains/cc:executor_host",
    command = """
export ASAN_SYMBOLIZER_PATH="$(location @linux_x86_64_clang_14_0_6//:llvm_symbolizer)"
{binary} $@
    """,
    data = [
        "@linux_x86_64_clang_14_0_6//:llvm_symbolizer",
    ],
    visibility = ["//visibility:public"],
)
