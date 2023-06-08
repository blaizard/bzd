load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "base.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container/ranges/views:all",
        "//cc/bzd/container/ranges/views:reverse",
        "//cc/bzd/container/ranges/views:stream",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility/bit",
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
        "//cc/bzd/container:optional",
        "//cc/bzd/container:span",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:remove_cvref",
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
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/container:span",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:remove_cvref",
        "//cc/bzd/utility:numeric_limits",
    ],
)
