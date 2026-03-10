load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "reader",
    srcs = ["reader_test.mjs"],
    main = "reader_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes/records",
        "//nodejs/core:exception",
        "//nodejs/core/mock:filesystem",
    ],
)

bzd_nodejs_test(
    name = "records",
    srcs = ["records_test.mjs"],
    main = "records_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes/records",
        "//nodejs/core:exception",
        "//nodejs/core/mock:filesystem",
    ],
)
