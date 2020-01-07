cc_test(
    name = "test",
    srcs = select({
        "@linux_x86_64_clang//:target": ["clang_test.cpp"],
        "//conditions:default": [],
    }),
    deps = [
        "@googletest//:gtest_main",
    ],
)
