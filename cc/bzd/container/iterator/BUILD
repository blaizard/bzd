load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "traits",
    hdrs = [
        "traits.hh",
    ],
    deps = [
        "//cc/bzd/type_traits:is_base_of",
        "//cc/bzd/type_traits:is_pointer",
        "//cc/bzd/type_traits:remove_pointer",
        "//cc/bzd/utility:concept",
    ],
)

cc_library(
    name = "random_access",
    hdrs = [
        "random_access.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":traits",
        "//cc/bzd/platform:types",
        "//cc/bzd/type_traits:conditional",
        "//cc/bzd/type_traits:is_same",
    ],
)

cc_library(
    name = "container_of_iterables",
    hdrs = [
        "container_of_iterables.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":traits",
        "//cc/bzd/container:optional",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "contiguous",
    hdrs = [
        "contiguous.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":random_access",
    ],
)

cc_library(
    name = "distance",
    hdrs = [
        "distance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":traits",
        "//cc/bzd/type_traits:is_iterator",
    ],
)

cc_library(
    name = "advance",
    hdrs = [
        "advance.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":traits",
        "//cc/bzd/type_traits:is_iterator",
    ],
)
