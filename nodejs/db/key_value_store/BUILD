load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "make_from_config",
    srcs = [
        "make_from_config.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":disk",
        ":elasticsearch",
        ":memory",
        ":mongodb",
        "//nodejs/core:validation",
    ],
)

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.mjs",
        "key_value_store.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:cache",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core/persistence:disk",
        "//nodejs/db:utils",
        "//nodejs/utils:uid",
    ],
)

bzd_nodejs_library(
    name = "elasticsearch",
    srcs = [
        "elasticsearch.mjs",
        "key_value_store.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
        "//nodejs/db:utils",
    ],
)

bzd_nodejs_library(
    name = "memory",
    srcs = [
        "key_value_store.mjs",
        "memory.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:log",
        "//nodejs/db:utils",
        "//nodejs/utils:uid",
    ],
)

bzd_nodejs_library(
    name = "mongodb",
    srcs = [
        "key_value_store.mjs",
        "mongodb.mjs",
    ],
    packages = [
        "@nodejs_deps//:mongodb",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/db:utils",
    ],
)
