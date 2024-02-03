load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "webhook_test",
    srcs = [
        "invoice.payment_succeeded.json",
        "webhook_test.mjs",
    ],
    main = "webhook_test.mjs",
    deps = [
        "//nodejs/payment/stripe:webhook",
    ],
)
