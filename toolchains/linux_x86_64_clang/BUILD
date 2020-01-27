load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "test",
    srcs = select({
        "@linux_x86_64_clang//:target": ["clang_test.cpp"],
        "//conditions:default": [],
    }),
    deps = [
        "//tools/cc_test",
    ],
)
