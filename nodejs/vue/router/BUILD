load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "router",
    srcs = [
        "router.mjs",
        "router_component.vue",
        "router_link.vue",
        "router_loading.vue",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:router",
        "//nodejs/vue/directives:loading",
    ],
)
