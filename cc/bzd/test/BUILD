load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "sync_point",
    hdrs = [
        "sync_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform",
    ],
)

cc_library(
    name = "inject_point",
    hdrs = [
        "inject_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_default_constructible",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/utility:forward",
    ],
)
