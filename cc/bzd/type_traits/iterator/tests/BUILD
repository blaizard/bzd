load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/test",
        "//cc/bzd/type_traits/iterator:begin",
        "//cc/bzd/type_traits/iterator:end",
    ],
) for path in glob([
    "*.cc",
])]
