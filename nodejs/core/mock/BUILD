load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

package(default_visibility = ["//visibility:public"])

bzd_nodejs_library(
    name = "filesystem",
    srcs = [
        "filesystem.mjs",
    ],
    deps = [
        "//nodejs/utils:pathlib",
    ],
)
