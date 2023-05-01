load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "base.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container/range:stream",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/container:string_view"
    ],
)

cc_library(
    name = "integral",
    hdrs = [
        "integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/container:array",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string_view",
        "//cc/bzd/container/range:stream",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:begin",
        "//cc/bzd/utility:end",
    ],
)

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
        "//cc/bzd/type_traits:range",
    ],
)

cc_library(
    name = "pattern",
    hdrs = [
        "pattern.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        ":integral",
        ":regexpr",
        "//cc/bzd/container:string",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/utility/pattern",
    ],
)
