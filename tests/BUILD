load("@rules_cc//cc:defs.bzl", "cc_test")

test_cpp_srcs = glob([
    "**/*_test.cpp",
])

[cc_test(
    name = src.replace(".cpp", ""),
    srcs = [src],
    deps = [
        "//platform:bzd",
        "//platform:sdk",
        "//tools/cc_test",
    ],
) for src in test_cpp_srcs]
