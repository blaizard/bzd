load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

test_srcs = glob([
    "*_test.mjs",
])

[bzd_nodejs_test(
    name = src.replace("_test.mjs", ""),
    srcs = [src],
    main = src,
    deps = [
        "//nodejs/core:cache",
        "//nodejs/core:cache2",
        "//nodejs/core:format",
        "//nodejs/core:router",
        "//nodejs/core:template",
        "//nodejs/core:timeseries",
        "//nodejs/core:validation",
        "//nodejs/core/cryptography:bcrypt",
        "//nodejs/core/persistence:disk",
        "//nodejs/core/persistence:timeseries",
    ],
) for src in test_srcs]
