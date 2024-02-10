load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "form",
    srcs = glob([
        "**/*.vue",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:script",
        "//nodejs/core:upload",
        "//nodejs/core:validation",
        "//nodejs/vue/directives:tooltip",
        "//nodejs/vue/directives:touch",
    ],
)
