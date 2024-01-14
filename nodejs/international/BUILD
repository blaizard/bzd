load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "countries",
    srcs = [
        "countries.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "currency",
    srcs = [
        "currency.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)

[bzd_nodejs_library(
    name = "flags_{}".format(size),
    srcs = [
        "flags_{}.png".format(size),
        "flags_{}.scss".format(size),
    ],
    visibility = ["//visibility:public"],
) for size in [
    "24x16",
    "32x24",
    "48x32",
]]
