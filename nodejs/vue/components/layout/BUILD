load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "layout",
    srcs = [
        "base.vue",
        "layout.vue",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:localstorage",
        "//nodejs/vue/directives:tooltip",
    ],
)

bzd_nodejs_library(
    name = "component",
    srcs = [
        "component.vue",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
    ],
)
