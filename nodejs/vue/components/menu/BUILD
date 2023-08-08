load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "entry",
    srcs = [
        "entry.vue",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:vue",
        "//nodejs/vue/directives:tooltip",
    ],
)
