load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
    name = "plugin",
    srcs = ["plugin_test.mjs"],
    main = "plugin_test.mjs",
    packages = [
        "@nodejs_deps//:fast-xml-parser",
    ],
    deps = [
        "//apps/artifacts/backend:plugin_tester",
        "//apps/artifacts/plugins/storage:backend",
        "//nodejs/core:exception",
    ],
)
