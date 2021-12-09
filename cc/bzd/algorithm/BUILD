load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "algorithm",
    visibility = ["//visibility:public"],
    deps = [
        ":binary_search",
        ":copy",
        ":copy_n",
        ":fill",
        ":lower_bound",
        ":reverse",
        ":sort",
        ":upper_bound"
    ],
)

# ---- Individual items ----

cc_library(
    name = "binary_search",
    hdrs = [
        "binary_search.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_iterator",
        ":lower_bound",
    ],
)

cc_library(
    name = "copy",
    hdrs = [
        "copy.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_iterator",
    ],
)

cc_library(
    name = "copy_n",
    hdrs = [
        "copy_n.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_iterator",
    ],
)

cc_library(
    name = "fill",
    hdrs = [
        "fill.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_iterator",
    ],
)

cc_library(
    name = "lower_bound",
    hdrs = [
        "lower_bound.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/iterator:advance",
        "//cc/bzd/container/iterator:distance",
        "//cc/bzd/type_traits:is_iterator",
        "//cc/bzd/utility/comparison"
    ],
)

cc_library(
    name = "reverse",
    hdrs = [
        "reverse.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_iterator",
        "//cc/bzd/utility:swap",
    ],
)

cc_library(
    name = "sort",
    hdrs = [
        "sort.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/iterator:distance",
        "//cc/bzd/type_traits:is_iterator",
        "//cc/bzd/type_traits:remove_cvref",
        "//cc/bzd/utility:swap",
        "//cc/bzd/utility/comparison",
    ],
)

cc_library(
    name = "upper_bound",
    hdrs = [
        "upper_bound.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/iterator:advance",
        "//cc/bzd/container/iterator:distance",
        "//cc/bzd/type_traits:is_iterator",
        "//cc/bzd/utility/comparison"
    ],
)
