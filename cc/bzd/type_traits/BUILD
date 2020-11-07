load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
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
        ":conditional",
        ":decay",
        ":declval",
        ":enable_if",
        ":false_type",
        ":integral_constant",
        ":invoke_result",
        ":is_arithmetic",
        ":is_array",
        ":is_const",
        ":is_constructible",
        ":is_destructible",
        ":is_floating_point",
        ":is_function",
        ":is_integral",
        ":is_lvalue_reference",
        ":is_pointer",
        ":is_reference",
        ":is_rvalue_reference",
        ":is_same",
        ":is_trivially_destructible",
        ":is_void",
        ":is_volatile",
        ":remove_const",
        ":remove_cv",
        ":remove_extent",
        ":remove_reference",
        ":remove_volatile",
        ":true_type",
        ":void_type",
    ],
)

# ---- Individual items ----

cc_library(
    name = "add_const",
    hdrs = [
        "add_const.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "add_cv",
    hdrs = [
        "add_cv.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "add_lvalue_reference",
    hdrs = [
        "add_lvalue_reference.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":add_reference",
    ],
)

cc_library(
    name = "add_pointer",
    hdrs = [
        "add_pointer.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":remove_reference",
    ],
)

cc_library(
    name = "add_reference",
    hdrs = [
        "add_reference.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "add_rvalue_reference",
    hdrs = [
        "add_rvalue_reference.h",
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
        "add_volatile.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "conditional",
    hdrs = [
        "conditional.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "decay",
    hdrs = [
        "decay.h",
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
        "declval.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":add_rvalue_reference",
    ],
)

cc_library(
    name = "enable_if",
    hdrs = [
        "enable_if.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "false_type",
    hdrs = [
        "false_type.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
    ],
)

cc_library(
    name = "integral_constant",
    hdrs = [
        "integral_constant.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "invoke_result",
    hdrs = [
        "invoke_result.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":decay",
        ":declval",
        ":enable_if",
        ":false_type",
        ":is_function",
        ":true_type",
        "//cc/bzd/container:reference_wrapper",
        "//cc/bzd/utility:forward",
    ],
)

cc_library(
    name = "is_arithmetic",
    hdrs = [
        "is_arithmetic.h",
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
        "is_array.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_const",
    hdrs = [
        "is_const.h",
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
        "is_constructible.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":declval",
        ":integral_constant",
        ":void_type",
    ],
)

cc_library(
    name = "is_destructible",
    hdrs = [
        "is_destructible.h",
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
    name = "is_floating_point",
    hdrs = [
        "is_floating_point.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":is_same",
        ":remove_cv",
    ],
)

cc_library(
    name = "is_function",
    hdrs = [
        "is_function.h",
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
        "is_integral.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":is_same",
        ":remove_cv",
    ],
)

cc_library(
    name = "is_lvalue_reference",
    hdrs = [
        "is_lvalue_reference.h",
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
        "is_pointer.h",
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
        "is_reference.h",
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
        "is_rvalue_reference.h",
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
        "is_same.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "is_trivially_destructible",
    hdrs = [
        "is_trivially_destructible.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
        ":is_destructible",
    ],
)

cc_library(
    name = "is_void",
    hdrs = [
        "is_void.h",
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
        "is_volatile.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":false_type",
        ":true_type",
    ],
)

cc_library(
    name = "remove_const",
    hdrs = [
        "remove_const.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "remove_cv",
    hdrs = [
        "remove_cv.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":remove_const",
        ":remove_volatile",
    ],
)

cc_library(
    name = "remove_extent",
    hdrs = [
        "remove_extent.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "remove_reference",
    hdrs = [
        "remove_reference.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "remove_volatile",
    hdrs = [
        "remove_volatile.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "true_type",
    hdrs = [
        "true_type.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral_constant",
    ],
)

cc_library(
    name = "void_type",
    hdrs = [
        "void_type.h",
    ],
    visibility = ["//visibility:public"],
)
