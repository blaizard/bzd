load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "graph",
    srcs = glob([
        "**/*",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/vue/directives:resize",
        "//nodejs/vue/directives:tooltip",
    ],
)
