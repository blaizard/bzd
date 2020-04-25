load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "form",
    srcs = glob([
        "form/**/*.vue",
        "form/**/*.scss",
        "form/**/*.svg",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:upload",
        "//nodejs/vue/directives:touch",
    ],
)
