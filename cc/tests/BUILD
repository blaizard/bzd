load("@rules_cc//cc:defs.bzl", "cc_test")

test_cpp_srcs = glob([
    "**/*_test.cpp",
])

[cc_test(
    name = src.replace("_test.cpp", ""),
    srcs = [src],
    deps = [
        "//cc:bzd",
        "//tools/cc_test",
    ],
) for src in test_cpp_srcs]
