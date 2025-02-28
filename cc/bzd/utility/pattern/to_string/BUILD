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
    name = "boolean",
    hdrs = [
        "boolean.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:string_view",
    ],
)

cc_library(
    name = "input_range",
    hdrs = [
        "input_range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:string",
        "//cc/bzd/container:string_view",
        "//cc/bzd/type_traits:is_convertible",
        "//cc/bzd/utility/ranges/views:take",
    ],
)

cc_library(
    name = "byte",
    hdrs = [
        "byte.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral",
    ],
)

cc_library(
    name = "enum",
    hdrs = [
        "enum.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_enum",
        "//cc/bzd/type_traits:underlying_type",
        ":integral",
    ],
)

cc_library(
    name = "floating_point",
    hdrs = [
        "floating_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        ":integral",
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:string_view",
        "//cc/bzd/type_traits:is_floating_point",
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
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:array",
        "//cc/bzd/container:string",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
        "//cc/bzd/utility/ranges/views:reverse",
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
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:string_view",
        "//cc/bzd/utility/pattern",
    ],
)

cc_library(
    name = "pointer",
    hdrs = [
        "pointer.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        ":integral",
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:string_view",
        "//cc/bzd/type_traits:is_convertible",
        "//cc/bzd/type_traits:is_pointer",
    ],
)
