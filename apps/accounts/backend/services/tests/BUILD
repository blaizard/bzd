load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "tests",
    srcs = [
        "services_test.mjs",
    ],
    main = "services_test.mjs",
    deps = [
        "//apps/accounts/backend/services",
        "//nodejs/core:exception",
    ],
)
