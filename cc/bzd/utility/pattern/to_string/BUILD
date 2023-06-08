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
        "//cc/bzd/container/ranges/views:stream",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/type_traits:is_same_class",
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
        "//cc/bzd/container/ranges/views:reverse",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
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

cc_library(
    name = "string",
    hdrs = [
        "string.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:string",
        "//cc/bzd/container:string_view",
        "//cc/bzd/container/ranges/views:take",
        "//cc/bzd/type_traits:is_convertible",
    ],
)