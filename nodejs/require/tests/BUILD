load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_test")

test_srcs = glob([
    "*_test.js",
])

[bzd_nodejs_test(
    name = src.replace(".js", ""),
    srcs = [src],
    main = src,
    deps = [
        "//nodejs/require",
    ],
) for src in test_srcs]
