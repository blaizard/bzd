load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "test",
    srcs = select({
        "@linux_x86_64_gcc//:target": ["gcc_test.cc"],
        "//conditions:default": [],
    }),
    deps = [
        "//tools/cc_test",
    ],
)
