load("//tools/bazel_build/rules:cc.bzl", "cc_test")

[cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/algorithm",
        "//cc/bzd/container:array",
        "//cc/bzd/container:string",
        "//tools/cc_test",
    ],
) for path in glob([
    "*.cc",
])]
