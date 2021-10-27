load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "integral",
    hdrs = [
        "integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm:reverse",
        "//cc/bzd/container:string",
        "//cc/bzd/container:vector",
        "//cc/bzd/core:channel",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
    ],
)

cc_library(
    name = "format",
    hdrs = [
        "format.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral",
        "//cc/bzd/container:array",
        "//cc/bzd/container:function_view",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string",
        "//cc/bzd/container:tuple",
        "//cc/bzd/container:vector",
        "//cc/bzd/core:channel",
        "//cc/bzd/meta:range",
        "//cc/bzd/type_traits:decay",
        "//cc/bzd/type_traits:declval",
        "//cc/bzd/type_traits:is_arithmetic",
        "//cc/bzd/type_traits:is_constructible",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_pointer",
        "//cc/bzd/utility:constexpr_for"
    ],
)
