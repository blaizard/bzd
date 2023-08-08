load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

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
