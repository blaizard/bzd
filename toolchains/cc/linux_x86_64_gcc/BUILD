load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "test",
    srcs = select({
        "@linux_x86_64_gcc//:target": ["gcc_test.cpp"],
        "//conditions:default": [],
    }),
    deps = [
        "//tools/cc_test",
    ],
)
