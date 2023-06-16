load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "container",
    visibility = ["//visibility:public"],
    deps = [
        ":any_reference",
        ":array",
        ":btree",
        ":function_ref",
        ":ichannel_buffered",
        ":map",
        ":named_type",
        ":non_owning_list",
        ":optional",
        ":ostream_buffered",
        ":pool",
        ":queue",
        ":reference_wrapper",
        ":result",
        ":ring_buffer",
        ":span",
        ":spans",
        ":stack",
        ":string",
        ":string_stream",
        ":string_view",
        ":value_wrapper",
        ":variant",
        ":vector",
        ":wrapper",
        "//cc/bzd/container/impl",
        "//cc/bzd/container/ranges:subrange",
        "//cc/bzd/container/threadsafe",
        "//cc/bzd/core/assert",
    ],
)

# ---- Individual items ----

cc_library(
    name = "any_reference",
    hdrs = [
        "any_reference.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":result",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_same",
    ],
)

cc_library(
    name = "array",
    hdrs = [
        "array.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/container/storage:fixed",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:in_place",
    ],
)

cc_library(
    name = "btree",
    hdrs = [
        "btree.hh",
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
    name = "function_ref",
    hdrs = [
        "function_ref.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":variant",
        "//cc/bzd/type_traits:add_pointer",
        "//cc/bzd/type_traits:invocable",
        "//cc/bzd/type_traits:is_same_class",
    ],
)

cc_library(
    name = "ichannel_buffered",
    hdrs = [
        "ichannel_buffered.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":optional",
        ":ring_buffer",
        ":span",
        "//cc/bzd/algorithm:copy",
        "//cc/bzd/container/ranges/views:drop",
        "//cc/bzd/container/ranges/views:reverse",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "map",
    hdrs = [
        "map.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":optional",
        ":tuple",
        ":vector",
        "//cc/bzd/algorithm:lower_bound",
        "//cc/bzd/algorithm:move_backward",
        "//cc/bzd/algorithm:sort",
        "//cc/bzd/algorithm:upper_bound",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility/comparison",
    ],
)

cc_library(
    name = "named_type",
    hdrs = [
        "named_type.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_arithmetic",
        "//cc/bzd/type_traits:is_floating_point",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/utility:move",
        "//cc/bzd/utility:ratio",
    ],
)

cc_library(
    name = "non_owning_list",
    hdrs = [
        "non_owning_list.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":optional",
        "//cc/bzd/container/iterator:bidirectional",
        "//cc/bzd/type_traits:add_const",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:is_const",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/utility:ignore",
    ],
)

cc_library(
    name = "optional",
    hdrs = [
        "optional.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":reference_wrapper",
        ":variant",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:decay",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:first_type",
        "//cc/bzd/type_traits:invocable",
        "//cc/bzd/type_traits:invoke_result",
        "//cc/bzd/type_traits:is_lvalue_reference",
        "//cc/bzd/type_traits:is_reference",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:is_same_class",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/type_traits:remove_volatile",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "ostream_buffered",
    hdrs = [
        "ostream_buffered.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        ":vector",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "pool",
    hdrs = [
        "pool.hh",
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
        "queue.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/container/impl:single_linked_pool",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "reference_wrapper",
    hdrs = [
        "reference_wrapper.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "result",
    hdrs = [
        "result.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:variant",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:decay",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_base_of",
        "//cc/bzd/type_traits:is_constructible",
        "//cc/bzd/type_traits:is_lvalue_reference",
        "//cc/bzd/type_traits:is_reference",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/type_traits:remove_const",
        "//cc/bzd/type_traits:remove_volatile",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "ring_buffer",
    hdrs = [
        "ring_buffer.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        ":spans",
        "//cc/bzd/algorithm:copy",
        "//cc/bzd/container/storage:fixed",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility:min",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "span",
    hdrs = [
        "span.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/impl:span",
        "//cc/bzd/container/storage:non_owning",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_const",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/type_traits:remove_const",
        "//cc/bzd/utility:swap",
    ],
)

cc_library(
    name = "spans",
    hdrs = [
        "spans.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":array",
        ":span",
        "//cc/bzd/container/iterator:container_of_iterables",
        "//cc/bzd/utility:apply",
        "//cc/bzd/utility:min",
    ],
)

cc_library(
    name = "stack",
    hdrs = [
        "stack.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm:fill",
        "//cc/bzd/container:array",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "string_stream",
    hdrs = [
        "string_stream.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":string",
        ":string_view",
        "//cc/bzd/core:channel",
        "//cc/bzd/type_traits:range",
    ],
)

cc_library(
    name = "string_view",
    hdrs = [
        "string_view.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/container/storage:non_owning",
        "//cc/bzd/meta:string_literal",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:is_base_of",
        "//cc/bzd/type_traits:range",
    ],
)

cc_library(
    name = "string",
    hdrs = [
        "string.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":optional",
        ":span",
        ":string_view",
        "//cc/bzd/algorithm:byte_copy",
        "//cc/bzd/algorithm:copy",
        "//cc/bzd/algorithm:fill_n",
        "//cc/bzd/container/impl:span_resizeable",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:add_const",
        "//cc/bzd/type_traits:is_trivially_copyable",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility:min",
    ],
)

cc_library(
    name = "tuple",
    hdrs = [
        "tuple.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":array",
        "//cc/bzd/container/iterator:tuple",
        "//cc/bzd/meta:choose_nth",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:enable_if",
        "//cc/bzd/type_traits:is_same",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:in_place",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "value_wrapper",
    hdrs = [
        "value_wrapper.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "variant",
    hdrs = [
        "variant.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":reference_wrapper",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/meta:type_list",
        "//cc/bzd/meta:union",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:add_reference",
        "//cc/bzd/type_traits:is_const",
        "//cc/bzd/type_traits:is_reference",
        "//cc/bzd/type_traits:is_trivially_copy_assignable",
        "//cc/bzd/type_traits:is_trivially_copy_constructible",
        "//cc/bzd/type_traits:is_trivially_destructible",
        "//cc/bzd/type_traits:is_trivially_move_assignable",
        "//cc/bzd/type_traits:is_trivially_move_constructible",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:in_place",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "vector",
    hdrs = [
        "vector.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":span",
        "//cc/bzd/algorithm:copy",
        "//cc/bzd/algorithm:move",
        "//cc/bzd/container/impl:span_resizeable",
        "//cc/bzd/container/storage:non_owning",
        "//cc/bzd/container/storage:resizeable",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:range",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility:in_place",
    ],
)

cc_library(
    name = "wrapper",
    hdrs = [
        "wrapper.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":reference_wrapper",
        ":value_wrapper",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:is_lvalue_reference",
    ],
)
