load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.mjs",
        "storage.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:stream",
        "//nodejs/db:utils",
    ],
)

bzd_nodejs_library(
    name = "google_cloud_storage",
    srcs = [
        "google_cloud_storage.mjs",
        "storage.mjs",
    ],
    packages = {
        "@google-cloud/storage": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:stream",
        "//nodejs/db:utils",
    ],
)

bzd_nodejs_library(
    name = "docker_v2",
    srcs = [
        "docker_v2.mjs",
        "storage.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:cache",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
        "//nodejs/db:utils",
    ],
)
