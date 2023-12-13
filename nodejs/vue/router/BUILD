load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "router",
    srcs = [
        "registry.mjs",
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
    ],
)
