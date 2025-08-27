load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "array",
    srcs = ["array_test.mjs"],
    main = "array_test.mjs",
    deps = [
        "//nodejs/core:exception",
        "//nodejs/utils:array",
    ],
)
