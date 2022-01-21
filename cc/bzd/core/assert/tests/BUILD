load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    target_compatible_with = select({
        "@linux_x86_64_gcc//:compiler": [],
        "@linux_x86_64_clang//:compiler": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//cc/bzd/core/assert",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
