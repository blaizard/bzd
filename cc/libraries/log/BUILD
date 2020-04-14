load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "log",
    visibility = ["//visibility:public"],
    deps = select({
        "//cc/libraries/log/stdout:backend": ["//cc/libraries/log/stdout"],
        "//conditions:default": ["//cc/libraries/log/stdout"],
    }),
)
