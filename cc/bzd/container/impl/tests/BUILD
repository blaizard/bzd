load("//tools/bazel_build/rules:cc.bzl", "cc_test")

[cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    linkopts = [
        "-pthread"
    ],
    deps = [
        "//cc/bzd/container/impl",
        "//tools/cc_test",
        "//cc/bzd/test:sync_point",
    ],
) for path in glob([
    "*.cc",
])]
