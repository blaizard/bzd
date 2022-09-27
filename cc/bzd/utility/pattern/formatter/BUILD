load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "integral",
    hdrs = [
        "integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm:reverse",
        "//cc/bzd/container:array",
        "//cc/bzd/container:string",
        "//cc/bzd/container:vector",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
    ],
)

cc_library(
    name = "to_string",
    hdrs = [
        "to_string.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral",
        "//cc/bzd/container:array",
        "//cc/bzd/container:string",
        "//cc/bzd/container:tuple",
        "//cc/bzd/meta:range",
        "//cc/bzd/meta:tuple",
        "//cc/bzd/type_traits:declval",
        "//cc/bzd/type_traits:is_arithmetic",
        "//cc/bzd/type_traits:is_constructible",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_pointer",
        "//cc/bzd/utility/pattern",
    ],
)

cc_library(
    name = "to_stream",
    hdrs = [
        "to_stream.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":to_string",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/utility/pattern:async",
    ],
)
