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

sh_binary_wrapper(
    name = "executor_host",
    binary = "//tools/bazel_build/toolchains/cc:executor_host",
    command = """
export ASAN_SYMBOLIZER_PATH="$(location @linux_x86_64_clang_12_0_0//:llvm_symbolizer)"
{binary} $@
    """,
    data = [
        "@linux_x86_64_clang_12_0_0//:llvm_symbolizer",
    ],
    visibility = ["//visibility:public"],
)
