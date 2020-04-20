load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.js",
    ],
    deps = [
        ":log",
        ":exception",
        ":fetch",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "log",
    srcs = [
        "log.js"
    ],
    deps = [
        ":format",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "exception",
    srcs = [
        "exception.js"
    ],
    deps = [
        ":format",
        ":log",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "event",
    srcs = [
        "event.js"
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "format",
    srcs = [
        "format.js"
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
        ":log",
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "fetch",
    srcs = [
        "fetch.js"
    ],
    deps = [
        ":exception",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "filesystem",
    srcs = [
        "filesystem.js"
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "localstorage",
    srcs = [
        "localstorage.js"
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "mutex",
    srcs = [
        "mutex.js"
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "router",
    srcs = [
        "router.js"
    ],
    deps = [
        ":exception",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "template",
    srcs = [
        "template.js"
    ],
    deps = [
        ":exception",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "timeseries",
    srcs = [
        "timeseries.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":log",
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "upload",
    srcs = [
        "upload.js"
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
        ":log",
        ":exception",
    ],
)
