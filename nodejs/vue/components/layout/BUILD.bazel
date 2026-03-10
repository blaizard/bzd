load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_config(
    name = "config",
    include_workspace_status = ["STABLE_VERSION"],
)

bzd_nodejs_library(
    name = "layout",
    srcs = [
        "base.vue",
        "layout.vue",
        ":config.json",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":accordion",
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:localstorage",
        "//nodejs/vue/directives:tooltip",
    ],
)

bzd_nodejs_library(
    name = "accordion",
    srcs = [
        "accordion.vue",
    ],
    visibility = ["//visibility:public"],
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
