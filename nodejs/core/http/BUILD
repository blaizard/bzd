load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
        "//nodejs/core/http/client:backend.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:crypto",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/utils:object",
    ],
)

bzd_nodejs_library(
    name = "endpoint",
    srcs = [
        "endpoint.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:router",
        "//nodejs/utils:regexpr",
    ],
)

bzd_nodejs_library(
    name = "server_context",
    srcs = [
        "server_context.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    packages = [
        "@nodejs_deps//:body-parser",
        "@nodejs_deps//:compression",
        "@nodejs_deps//:cookie-parser",
        "@nodejs_deps//:express",
        "@nodejs_deps//:express-minify",
        "@nodejs_deps//:helmet",
        "@nodejs_deps//:http",
        "@nodejs_deps//:mime",
        "@nodejs_deps//:multer",
        "@nodejs_deps//:ws",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":endpoint",
        ":server_context",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:router",
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
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
