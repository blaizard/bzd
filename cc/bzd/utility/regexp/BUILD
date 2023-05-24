load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "regexp",
    hdrs = [
        "matcher_brackets.hh",
        "matcher_single_char.hh",
        "regexp.hh",
        "types.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:function_ref",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/container:string_view",
        "//cc/bzd/container/range:stream",
        "//cc/bzd/type_traits:is_reference",
        "//cc/bzd/type_traits:range",
    ],
)

cc_library(
    name = "async",
    hdrs = [
        "async.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":regexp",
        "//cc/bzd/core/async",
    ],
)
