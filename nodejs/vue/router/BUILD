load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "router",
    srcs = [
        "router.js",
        "router.vue",
        "router_component.vue",
        "router_link.vue",
    ],
    packages = {
        "vue": "",
    },
    deps = [
        "//nodejs/core:router",
    ],
    visibility = ["//visibility:public"],
)
