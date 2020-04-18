load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_test")

test_srcs = glob([
    "*_test.js",
])

[bzd_nodejs_test(
    name = src.replace("_test.js", ""),
    srcs = [src],
    main = src,
    deps = [
        "//nodejs/core/persistence:disk",
        "//nodejs/core/persistence:timeseries",
        "//nodejs/core:template",
        "//nodejs/core:timeseries",
        "//nodejs/core:format",
    ],
) for src in test_srcs]
