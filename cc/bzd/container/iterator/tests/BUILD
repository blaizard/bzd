load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/container/iterator:begin",
        "//cc/bzd/container/iterator:end",
        "//cc/bzd/test",
    ],
) for path in glob([
    "*.cc",
])]
