load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "records",
    srcs = [
        "records.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:lock",
        "//nodejs/core:log",
        "//nodejs/core/statistics:provider",
    ],
)

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
        "data_generator.mjs",
        "storage.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":data",
        ":key_mapping",
        ":nodes",
        ":records",
        "//apps/artifacts/backend:plugin",
        "//apps/artifacts/common:utils",
        "//apps/artifacts/plugins/nodes/databases:influxdb",
        "//apps/artifacts/plugins/nodes/sources:nodes",
        "//nodejs/core:cache",
        "//nodejs/core:exception",
        "//nodejs/core:format",
        "//nodejs/core:log",
        "//nodejs/core:router",
        "//nodejs/core/http:client",
        "//nodejs/core/services",
        "//nodejs/core/statistics:provider",
        "//nodejs/db:utils",
        "//nodejs/db/key_value_store:make_from_config",
        "//nodejs/db/storage",
        "//nodejs/utils:object",
        "//nodejs/utils:optional",
    ],
)

bzd_nodejs_library(
    name = "nodes",
    srcs = [
        "nodes.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":data",
        "//apps/artifacts/common:utils",
        "//apps/artifacts/plugins/nodes/handlers",
        "//nodejs/core:cache2",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "data",
    srcs = [
        "data.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":key_mapping",
        "//apps/artifacts/common:utils",
        "//nodejs/core:cache2",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/statistics:provider",
        "//nodejs/utils:optional",
    ],
)

bzd_nodejs_library(
    name = "key_mapping",
    srcs = [
        "key_mapping.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
)

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "frontend.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__pkg__",
    ],
    deps = [
        "//apps/artifacts/plugins:default",
        "//apps/artifacts/plugins/nodes/frontend:view",
    ],
)
