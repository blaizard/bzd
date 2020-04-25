load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "router",
    srcs = [
        "router.js",
        "router_component.vue",
        "router_link.vue",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:router",
    ],
)
