load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "graph",
    srcs = glob([
        "**/*.vue",
        "**/*.mjs",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/vue/directives:resize",
        "//nodejs/vue/directives:tooltip",
    ],
)
