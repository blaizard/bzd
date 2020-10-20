load("@rules_cc//cc:defs.bzl", "cc_test")

test_cpp_srcs = glob([
    "**/*_test.cc",
])

[cc_test(
    name = src.replace("_test.cc", ""),
    srcs = [src],
    deps = [
        "//cc:bzd",
        "//tools/cc_test",
    ],
) for src in test_cpp_srcs]
