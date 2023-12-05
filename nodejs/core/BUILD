load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "authentication",
    srcs = [
        "authentication.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "log",
    srcs = [
        "log.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":format",
    ],
)

bzd_nodejs_library(
    name = "exception",
    srcs = [
        "exception.mjs",
        "//nodejs/core/impl/exception:backend.mjs",
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
        "event.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "format",
    srcs = [
        "format.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "cache",
    srcs = [
        "cache.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":event",
        ":exception",
        ":log",
    ],
)

bzd_nodejs_library(
    name = "cookie",
    srcs = [
        "cookie.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "crypto",
    srcs = [
        "crypto.mjs",
        "//nodejs/core/impl/crypto:backend.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "filesystem",
    srcs = [
        "filesystem.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "localstorage",
    srcs = [
        "localstorage.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "mutex",
    srcs = [
        "mutex.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "router",
    srcs = [
        "router.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
        "//nodejs/utils:regexpr",
    ],
)

bzd_nodejs_library(
    name = "script",
    srcs = [
        "script.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "stream",
    srcs = [
        "stream.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "template",
    srcs = [
        "template.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
    ],
)

bzd_nodejs_library(
    name = "timeseries",
    srcs = [
        "timeseries.mjs",
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
        "upload.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "validation",
    srcs = [
        "validation.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":exception",
        ":format",
    ],
)
