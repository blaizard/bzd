load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "permissions",
    srcs = [
        "permissions.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
    ],
)

bzd_nodejs_library(
    name = "storage",
    srcs = [
        "storage.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":permissions",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:stream",
        "//nodejs/db:utils",
    ],
)

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.mjs",
        "storage.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":storage",
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
    ],
    packages = {
        "@google-cloud/storage": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        ":storage",
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
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":storage",
        "//nodejs/core:cache",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
        "//nodejs/db:utils",
    ],
)
