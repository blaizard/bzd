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
    ],
)

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
        "nodes.mjs",
        "storage.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":data",
        ":key_mapping",
        ":records",
        "//apps/artifacts/backend:plugin",
        "//apps/artifacts/plugins/nodes/handlers",
        "//apps/artifacts/plugins/nodes/sinks:influxdb",
        "//nodejs/core:cache",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
        "//nodejs/core/services",
        "//nodejs/db:utils",
        "//nodejs/db/key_value_store:make_from_config",
        "//nodejs/db/storage",
        "//nodejs/utils:optional",
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
        "//nodejs/core:cache",
        "//nodejs/core:exception",
        "//nodejs/core:log",
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
        ":view",
        "//apps/artifacts/plugins:default",
    ],
)

bzd_nodejs_library(
    name = "view",
    srcs = [
        "keys.vue",
        "value.vue",
        "view_category.vue",
        "view_node.vue",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//apps/artifacts/plugins:default",
        "//nodejs/utils:to_string",
        "//nodejs/vue/directives:tooltip",
    ],
)
