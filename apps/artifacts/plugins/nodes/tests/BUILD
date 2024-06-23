load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "nodes",
    srcs = ["nodes_test.mjs"],
    main = "nodes_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes:backend",
        "//nodejs/core:exception",
    ],
)
