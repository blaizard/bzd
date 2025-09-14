load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "influxdb",
    srcs = ["influxdb_test.mjs"],
    main = "influxdb_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes/databases:influxdb",
        "//nodejs/core:exception",
    ],
)
