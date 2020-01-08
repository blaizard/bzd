test_cpp_srcs = glob([
    "**/*_test.cpp",
])

[cc_test(
    name = src.replace(".cpp", ""),
    srcs = [src],
    deps = [
        "//platform:bzd",
        "@googletest//:gtest_main",
    ],
) for src in test_cpp_srcs]
