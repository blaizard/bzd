load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "regexpr",
    hdrs = [
        "regexpr.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:function_ref",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/container:string_view",
        "//cc/bzd/container/range:stream",
        "//cc/bzd/type_traits:range",
    ],
)
