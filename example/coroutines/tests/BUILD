load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "tests",
    srcs = [
        "coroutine.cc",
    ],
    target_compatible_with = select({
        "@linux_x86_64_gcc//:target": [
            "@linux_x86_64_gcc//:compiler",
        ],
        "@linux_x86_64_clang//:target": [
            "@linux_x86_64_clang//:compiler",
        ],
        "//conditions:default": ["@linux_x86_64_clang//:compiler"],
    }),
    deps = [
        "//cc/bzd",
        "//example/coroutines:async",
        "//tools/cc_test",
    ],
    copts = [
        # With gcc there seems to be a flase positive on co_return, to be checked.
        "-Wno-unused-value",
    ]
)
