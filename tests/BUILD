test_cpp_srcs = glob([
    "**/*_test.cpp",
])

[cc_test(
    name = "test_" + src.replace("_test.cpp", ""),
    srcs = [src],
    deps = [
        "//src:bzd",
        "@googletest//:gtest_main",
    ],
) for src in test_cpp_srcs]
