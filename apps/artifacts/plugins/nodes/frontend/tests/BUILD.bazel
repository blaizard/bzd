load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "timeseries_collection",
    srcs = ["timeseries_collection_test.mjs"],
    main = "timeseries_collection_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes/frontend:timeseries_collection",
        "//nodejs/core:exception",
    ],
)
