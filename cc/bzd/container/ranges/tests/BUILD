load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [
        path,
    ],
    deps = [
        "//cc/bzd/algorithm:equal",
        "//cc/bzd/container",
        "//cc/bzd/container:ichannel_buffered",
        "//cc/bzd/container/ranges/views",
        "//cc/bzd/container/ranges/views_async",
        "//cc/bzd/test",
        "//cc/bzd/test/types:generator",
        "//cc/bzd/test/types:ichannel",
        "//cc/bzd/test/types:move_only",
        "//cc/bzd/test/types:range",
    ],
) for path in glob([
    "*.cc",
])]
