load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "filesystem",
    srcs = [
        "filesystem_test.mjs",
    ],
    main = "filesystem_test.mjs",
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core/mock:filesystem",
    ],
)
