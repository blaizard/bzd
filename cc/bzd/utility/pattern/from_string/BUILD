load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "base.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string_view",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/utility/ranges:stream",
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
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility/ranges:begin",
        "//cc/bzd/utility/ranges:end",
        "//cc/bzd/utility/ranges:stream",
    ],
)

cc_library(
    name = "range_of_ranges",
    hdrs = [
        "range_of_ranges.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/algorithm:equal_range",
        "//cc/bzd/container:map",
        "//cc/bzd/type_traits:is_same_template",
        "//cc/bzd/type_traits:range",
    ],
)

cc_library(
    name = "range",
    hdrs = [
        "range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility/regexp",
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
        "//cc/bzd/container:string",
        "//cc/bzd/container:variant",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/utility/pattern",
        "//cc/bzd/utility/regexp",
    ],
)
