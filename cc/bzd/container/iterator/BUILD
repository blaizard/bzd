load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "random_access",
    srcs = [
        "traits.hh",
    ],
    hdrs = [
        "random_access.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:is_same",
    ],
)

cc_library(
    name = "contiguous",
    hdrs = [
        "contiguous.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":random_access",
    ],
)

cc_library(
    name = "distance",
    srcs = [
        "traits.hh",
    ],
    hdrs = [
        "distance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_base_of",
    ],
)
