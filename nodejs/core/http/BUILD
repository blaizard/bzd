load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = glob([
        "client.mjs",
        "client/*.mjs",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
        "//nodejs/core:crypto",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    packages = {
        "express": "",
        "express-minify": "",
        "helmet": "",
        "http": "",
        "compression": "",
        "multer": "",
        "cookie-parser": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "proxy",
    srcs = [
        "proxy.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":client",
        "//nodejs:bzd",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
