load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "test",
    srcs = ["clang_test.cc"],
    target_compatible_with = [
        "@esp32_xtensa_lx6_clang//:compiler",
    ],
    deps = [
        "//cc/bzd/test",
    ],
)
