load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "scopes",
    srcs = ["scopes_test.mjs"],
    main = "scopes_test.mjs",
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core/authentication:scopes",
    ],
)
