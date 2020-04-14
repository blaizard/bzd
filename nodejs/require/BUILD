load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "require",
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":persistence",
        ":task",
        ":template",
        ":timeseries",
        ":web",
    ],
)

bzd_nodejs_library(
    name = "core",
    srcs = [
        "event.js",
        "exception.js",
        "filesystem.js",
        "log.js",
        "mutex.js",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "template",
    srcs = [
        "template.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
    ],
)

bzd_nodejs_library(
    name = "timeseries",
    srcs = [
        "timeseries.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":core",
    ],
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
        ":core",
    ],
)

bzd_nodejs_library(
    name = "task",
    srcs = glob([
        "task/*.js",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":core",
    ],
)

bzd_nodejs_library(
    name = "persistence",
    srcs = glob([
        "persistence/*.js",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        ":core",
        ":task",
    ],
)
