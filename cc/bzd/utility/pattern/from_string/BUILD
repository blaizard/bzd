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
        "//cc/bzd/container/range/views:stream",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/type_traits:is_same_class",
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
        "//cc/bzd/container/range/views:stream",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:begin",
        "//cc/bzd/utility:end",
    ],
)

cc_library(
    name = "output_range",
    hdrs = [
        "output_range.hh",
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
