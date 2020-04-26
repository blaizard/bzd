load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
        ":fetch",
        ":log",
    ],
)

bzd_nodejs_library(
    name = "log",
    srcs = [
        "log.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":format",
    ],
)

bzd_nodejs_library(
    name = "exception",
    srcs = [
        "exception.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":format",
        ":log",
    ],
)

bzd_nodejs_library(
    name = "event",
    srcs = [
        "event.js",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "format",
    srcs = [
        "format.js",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "cache",
    srcs = [
        "cache.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":event",
        ":exception",
        ":log",
    ],
)

bzd_nodejs_library(
    name = "crypto",
    srcs = glob([
        "crypto.js",
        "impl/crypto/*",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "fetch",
    srcs = glob([
        "fetch.js",
        "impl/fetch/*",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":crypto",
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "filesystem",
    srcs = [
        "filesystem.js",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "localstorage",
    srcs = [
        "localstorage.js",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "mutex",
    srcs = [
        "mutex.js",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "router",
    srcs = [
        "router.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "template",
    srcs = [
        "template.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "timeseries",
    srcs = [
        "timeseries.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
        ":log",
    ],
)

bzd_nodejs_library(
    name = "upload",
    srcs = [
        "upload.js",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "web",
    srcs = glob([
        "web.js",
    ]),
    packages = {
        "express": "",
        "express-minify": "",
        "helmet": "",
        "body-parser": "",
        "http": "",
        "compression": "",
        "multer": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
        ":log",
    ],
)
