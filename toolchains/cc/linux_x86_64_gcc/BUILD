load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "test",
    srcs = ["gcc_test.cc"],
    target_compatible_with = [
        "@linux_x86_64_gcc//:compiler"
    ],
    deps = [
        "//tools/cc_test",
    ],
)
