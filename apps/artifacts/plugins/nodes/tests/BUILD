load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "nodes",
    srcs = ["nodes_test.mjs"],
    main = "nodes_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes:backend",
        "//apps/artifacts/plugins/nodes:key_mapping",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_test(
    name = "data",
    srcs = ["data_test.mjs"],
    main = "data_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes:data",
        "//nodejs/core:exception",
        "//nodejs/db/key_value_store:make_from_config",
    ],
)

bzd_nodejs_test(
    name = "plugin",
    srcs = ["plugin_test.mjs"],
    main = "plugin_test.mjs",
    deps = [
        "//apps/artifacts/backend:plugin_tester",
        "//apps/artifacts/plugins/nodes:backend",
        "//nodejs/core:exception",
        "//nodejs/core/http/mock:client",
        "//nodejs/utils:delay",
    ],
)

bzd_nodejs_test(
    name = "records",
    srcs = ["records_test.mjs"],
    main = "records_test.mjs",
    deps = [
        "//apps/artifacts/plugins/nodes:records",
        "//nodejs/core:exception",
        "//nodejs/core/mock:filesystem",
    ],
)
