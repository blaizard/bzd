load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "impl",
    visibility = ["//visibility:public"],
    deps = [
        ":single_linked_pool",
        ":span",
        ":span_resizeable",
    ],
)

# ---- Individual items ----

cc_library(
    name = "single_linked_pool",
    hdrs = [
        "single_linked_pool.hh",
    ],
    visibility = ["//cc/bzd/container:__pkg__"],
    deps = [
        "//cc/bzd/container:span",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "span",
    hdrs = [
        "span.hh",
    ],
    visibility = ["//cc/bzd/container:__pkg__"],
    deps = [
        "//cc/bzd/algorithm:equal",
        "//cc/bzd/algorithm:find",
        "//cc/bzd/algorithm:lexicographical_compare",
        "//cc/bzd/algorithm:rsearch",
        "//cc/bzd/algorithm:search",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_const",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/utility:construct_at",
        "//cc/bzd/utility:destroy_at",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:move",
        "//cc/bzd/utility/iterators:contiguous",
        "//cc/bzd/utility/iterators:distance",
    ],
)

cc_library(
    name = "span_resizeable",
    hdrs = [
        "span_resizeable.hh",
    ],
    visibility = ["//cc/bzd/container:__pkg__"],
    deps = [
        ":span",
        "//cc/bzd/type_traits:is_convertible",
        "//cc/bzd/type_traits:is_trivially_copyable",
        "//cc/bzd/type_traits:is_trivially_default_constructible",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/type_traits:sentinel_for",
        "//cc/bzd/utility/iterators:input_or_output_reference",
        "//cc/bzd/utility/ranges:stream",
        "//cc/bzd/utility/ranges:subrange",
    ],
)
