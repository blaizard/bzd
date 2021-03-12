load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "test",
    target_compatible_with = [
        "@linux_x86_64_clang//:compiler"
    ],
    srcs = ["clang_test.cc"],
    deps = [
        "//tools/cc_test",
    ],
)
