load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "file_explorer",
    srcs = [
        "file_explorer.vue",
        "tree.vue",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/db/storage:permissions",
        "//nodejs/utils:to_string",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/directives:loading",
    ],
)
