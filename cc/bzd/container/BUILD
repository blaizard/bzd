load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "container",
    visibility = ["//visibility:public"],
    deps = [
        ":any_reference",
        ":array",
        ":btree",
        ":function",
        ":function_view",
        ":map",
        ":named_type",
        ":optional",
        ":pool",
        ":queue",
        ":reference_wrapper",
        ":result",
        ":span",
        ":string",
        ":string_channel",
        ":string_view",
        ":variant",
        ":vector",
        "//cc/bzd/core:assert_minimal",
    ],
)

# ---- Individual items ----

cc_library(
    name = "any_reference",
    hdrs = [
        "any_reference.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":result",
        "//cc/bzd/core:assert_minimal_header",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_same",
    ],
)

cc_library(
    name = "array",
    hdrs = [
        "array.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "btree",
    hdrs = [
        "btree.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":pool",
        ":tuple",
        ":vector",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "function",
    hdrs = [
        "function.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:add_pointer",
        "//cc/bzd/type_traits:decay",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "function_view",
    hdrs = [
        "function_view.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":function",
        ":variant",
        "//cc/bzd/type_traits:add_pointer",
        "//cc/bzd/utility:ignore",
    ],
)

cc_library(
    name = "map",
    hdrs = [
        "map.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":optional",
        ":vector",
        "//cc/bzd/core:assert_minimal_header",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "named_type",
    hdrs = [
        "named_type.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_arithmetic",
        "//cc/bzd/utility:move",
        "//cc/bzd/utility:ratio",
    ],
)

cc_library(
    name = "optional",
    hdrs = [
        "optional.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":reference_wrapper",
        "//cc/bzd/core:assert_minimal_header",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:is_reference",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "pool",
    hdrs = [
        "pool.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/container/impl:single_linked_pool",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:container_of",
    ],
)

cc_library(
    name = "queue",
    hdrs = [
        "queue.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/container/impl:single_linked_pool",
        "//cc/bzd/core:assert_minimal_header",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "reference_wrapper",
    hdrs = [
        "reference_wrapper.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "result",
    hdrs = [
        "result.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":reference_wrapper",
        "//cc/bzd/core:assert_minimal_header",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:decay",
        "//cc/bzd/type_traits:is_reference",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/type_traits:is_constructible",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "span",
    hdrs = [
        "span.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/iterator:contiguous",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_const",
        "//cc/bzd/type_traits:remove_const",
        "//cc/bzd/utility:swap",
    ],
)

cc_library(
    name = "string_channel",
    hdrs = [
        "string_channel.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":string",
        ":string_view",
        "//cc/bzd/core:channel",
    ],
)

cc_library(
    name = "string_view",
    hdrs = [
        "string_view.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "string",
    hdrs = [
        "string.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        ":string_view",
        "//cc/bzd/algorithm:copy",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:add_const",
        "//cc/bzd/utility:min",
    ],
)

cc_library(
    name = "tuple",
    hdrs = [
        "tuple.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/meta:choose_nth",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "variant",
    hdrs = [
        "variant.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":result",
        "//cc/bzd/core:assert_minimal_header",
        "//cc/bzd/meta:type_list",
        "//cc/bzd/meta:union",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_constructible",
    ],
)

cc_library(
    name = "vector",
    hdrs = [
        "vector.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/core:assert_minimal_header",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:forward",
    ],
)
