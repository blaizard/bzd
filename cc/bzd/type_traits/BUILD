load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "type_traits",
    visibility = ["//visibility:public"],
    deps = [
        ":add_const",
        ":add_cv",
        ":add_lvalue_reference",
        ":add_pointer",
        ":add_reference",
        ":add_rvalue_reference",
        ":add_volatile",
        ":assignable_from",
        ":async",
        ":conditional",
        ":container",
        ":decay",
        ":declval",
        ":derived_from",
        ":enable_if",
        ":false_type",
        ":first_type",
        ":function",
        ":integral_constant",
        ":invoke_result",
        ":is_arithmetic",
        ":is_array",
        ":is_base_of",
        ":is_class",
        ":is_const",
        ":is_constructible",
        ":is_convertible",
        ":is_default_constructible",
        ":is_destructible",
        ":is_enum",
        ":is_floating_point",
        ":is_function",
        ":is_integral",
        ":is_lvalue_reference",
        ":is_pointer",
        ":is_reference",
        ":is_rvalue_reference",
        ":is_same",
        ":is_same_class",
        ":is_same_template",
        ":is_signed",
        ":is_trivially_copy_assignable",
        ":is_trivially_copy_constructible",
        ":is_trivially_copyable",
        ":is_trivially_destructible",
        ":is_trivially_move_assignable",
        ":is_trivially_move_constructible",
        ":is_union",
        ":is_void",
        ":is_volatile",
        ":iterator",
        ":lockable",
        ":predicate",
        ":range",
        ":remove_const",
        ":remove_cv",
        ":remove_cvref",
        ":remove_extent",
        ":remove_pointer",
        ":remove_reference",
        ":remove_volatile",
        ":sentinel_for",
        ":sized_sentinel_for",
        ":true_type",
        ":underlying_type",
        ":void_type",
        ":weakly_equality_comparable_with",
    ],
)

# ---- Individual items ----

cc_library(
    name = "add_const",
    hdrs = [
        "add_const.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "add_cv",
    hdrs = [
        "add_cv.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "add_lvalue_reference",
    hdrs = [
        "add_lvalue_reference.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":add_reference",
    ],
)

cc_library(
    name = "add_pointer",
    hdrs = [
        "add_pointer.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":remove_reference",
    ],
)

cc_library(
    name = "add_reference",
    hdrs = [
        "add_reference.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "add_rvalue_reference",
    hdrs = [
        "add_rvalue_reference.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_reference",
        ":is_void",
    ],
)

cc_library(
    name = "add_volatile",
    hdrs = [
        "add_volatile.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "assignable_from",
    hdrs = [
        "assignable_from.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_same",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "async",
    hdrs = [
        "async.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_same_class",
        ":is_same_template",
    ],
)

cc_library(
    name = "conditional",
    hdrs = [
        "conditional.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "container",
    hdrs = [
        "container.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_convertible",
        "//cc/bzd/container:span",
    ],
)

cc_library(
    name = "decay",
    hdrs = [
        "decay.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":add_pointer",
        ":conditional",
        ":is_array",
        ":is_function",
        ":remove_cv",
        ":remove_extent",
        ":remove_reference",
    ],
)

cc_library(
    name = "declval",
    hdrs = [
        "declval.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":add_rvalue_reference",
    ],
)

cc_library(
    name = "derived_from",
    hdrs = [
        "derived_from.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_base_of",
        ":is_convertible",
    ],
)

cc_library(
    name = "enable_if",
    hdrs = [
        "enable_if.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "false_type",
    hdrs = [
        "false_type.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
    ],
)

cc_library(
    name = "first_type",
    hdrs = [
        "first_type.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "function",
    hdrs = [
        "function.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/meta:always_false",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "integral_constant",
    hdrs = [
        "integral_constant.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "invocable",
    hdrs = [
        "invocable.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_same",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "invoke_result",
    hdrs = [
        "invoke_result.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":decay",
        ":declval",
        ":enable_if",
        ":false_type",
        ":is_base_of",
        ":is_function",
        ":true_type",
        "//cc/bzd/container:reference_wrapper",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "is_arithmetic",
    hdrs = [
        "is_arithmetic.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":is_floating_point",
        ":is_integral",
    ],
)

cc_library(
    name = "is_array",
    hdrs = [
        "is_array.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_base_of",
    hdrs = [
        "is_base_of.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":integral_constant",
        ":is_class",
        ":true_type",
    ],
)

cc_library(
    name = "is_class",
    hdrs = [
        "is_class.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":integral_constant",
        ":is_union",
    ],
)

cc_library(
    name = "is_const",
    hdrs = [
        "is_const.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_constructible",
    hdrs = [
        "is_constructible.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":declval",
        ":integral_constant",
        ":void_type",
    ],
)

cc_library(
    name = "is_convertible",
    hdrs = [
        "is_convertible.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":declval",
        ":integral_constant",
        ":void_type",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "is_default_constructible",
    hdrs = [
        "is_default_constructible.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
        ":void_type",
    ],
)

cc_library(
    name = "is_destructible",
    hdrs = [
        "is_destructible.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":declval",
        ":false_type",
        ":integral_constant",
        ":true_type",
    ],
)

cc_library(
    name = "is_enum",
    hdrs = [
        "is_enum.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":remove_cv",
    ],
)

cc_library(
    name = "is_floating_point",
    hdrs = [
        "is_floating_point.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":is_same",
        ":remove_cvref",
    ],
)

cc_library(
    name = "is_function",
    hdrs = [
        "is_function.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_integral",
    hdrs = [
        "is_integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":is_same",
        ":remove_cvref",
    ],
)

cc_library(
    name = "is_lvalue_reference",
    hdrs = [
        "is_lvalue_reference.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_pointer",
    hdrs = [
        "is_pointer.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":remove_cv",
        ":true_type",
    ],
)

cc_library(
    name = "is_reference",
    hdrs = [
        "is_reference.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":is_lvalue_reference",
        ":is_rvalue_reference",
    ],
)

cc_library(
    name = "is_rvalue_reference",
    hdrs = [
        "is_rvalue_reference.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_same",
    hdrs = [
        "is_same.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_same_class",
    hdrs = [
        "is_same_class.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_same",
        ":remove_cvref",
    ],
)

cc_library(
    name = "is_same_template",
    hdrs = [
        "is_same_template.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":remove_cvref",
        ":true_type",
    ],
)

cc_library(
    name = "is_signed",
    hdrs = [
        "is_signed.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":integral_constant",
        ":is_arithmetic",
    ],
)

cc_library(
    name = "is_trivially_copy_assignable",
    hdrs = [
        "is_trivially_copy_assignable.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "is_trivially_copy_constructible",
    hdrs = [
        "is_trivially_copy_constructible.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "is_trivially_copyable",
    hdrs = [
        "is_trivially_copyable.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "is_trivially_destructible",
    hdrs = [
        "is_trivially_destructible.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "is_trivially_move_assignable",
    hdrs = [
        "is_trivially_move_assignable.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "is_trivially_move_constructible",
    hdrs = [
        "is_trivially_move_constructible.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "is_union",
    hdrs = [
        "is_union.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":remove_cv",
    ],
)

cc_library(
    name = "is_void",
    hdrs = [
        "is_void.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_volatile",
    hdrs = [
        "is_volatile.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "iterator",
    hdrs = [
        "iterator.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":derived_from",
        ":false_type",
        ":is_base_of",
        ":is_pointer",
        ":remove_pointer",
        ":true_type",
        ":void_type",
        "//cc/bzd/utility:to_underlying",
    ],
)

cc_library(
    name = "lockable",
    hdrs = [
        "lockable.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "predicate",
    hdrs = [
        "predicate.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":invocable",
        ":is_convertible",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "range",
    hdrs = [
        "range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":async",
        ":declval",
        ":is_lvalue_reference",
        ":is_trivially_copyable",
        ":iterator",
        ":remove_cvref",
        "//cc/bzd/utility:begin",
        "//cc/bzd/utility:end",
        "//cc/bzd/utility:size",
    ],
)

cc_library(
    name = "remove_const",
    hdrs = [
        "remove_const.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "remove_cv",
    hdrs = [
        "remove_cv.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":remove_const",
        ":remove_volatile",
    ],
)

cc_library(
    name = "remove_cvref",
    hdrs = [
        "remove_cvref.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":remove_cv",
        ":remove_reference",
    ],
)

cc_library(
    name = "remove_extent",
    hdrs = [
        "remove_extent.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "remove_pointer",
    hdrs = [
        "remove_pointer.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "remove_reference",
    hdrs = [
        "remove_reference.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "remove_volatile",
    hdrs = [
        "remove_volatile.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sentinel_for",
    hdrs = [
        "sentinel_for.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":iterator",
        ":weakly_equality_comparable_with",
    ],
)

cc_library(
    name = "sized_sentinel_for",
    hdrs = [
        "sized_sentinel_for.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_same",
        ":iterator",
        ":sentinel_for",
    ],
)

cc_library(
    name = "true_type",
    hdrs = [
        "true_type.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
    ],
)

cc_library(
    name = "underlying_type",
    hdrs = [
        "underlying_type.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "void_type",
    hdrs = [
        "void_type.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "weakly_equality_comparable_with",
    hdrs = [
        "weakly_equality_comparable_with.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":is_convertible",
    ],
)
