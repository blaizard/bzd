load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    target_compatible_with = select({
        "@gcc//:toolchain": [],
        "@clang//:toolchain": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//cc/bzd/core/assert",
        "//cc/bzd/platform:panic",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
