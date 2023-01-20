load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "types",
    hdrs = [
        "types.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container",
        "//cc/bzd/core:error",
        "//cc/bzd/core/io",
        "//cc/bzd/core/logger",
        "//cc/bzd/meta:string_literal",
        "//cc/bzd/type_traits:derived_from",
        "//cc/bzd/type_traits:function",
        "//cc/bzd/type_traits:is_same",
    ],
)
