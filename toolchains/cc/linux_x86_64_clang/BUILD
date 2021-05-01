load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "test",
    srcs = ["clang_test.cc"],
    target_compatible_with = [
        "@linux_x86_64_clang//:compiler",
    ],
    deps = [
        "//tools/cc_test",
    ],
)
