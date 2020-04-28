load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "form",
    srcs = glob([
        "form/**/*",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:upload",
        "//nodejs/vue/directives:touch",
    ],
)

bzd_nodejs_library(
    name = "graph",
    srcs = glob([
        "graph/**/*",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/vue/directives:resize",
    ],
)
