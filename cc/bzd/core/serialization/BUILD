load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "serialization",
    hdrs = [
        "serialization.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/range:stream",
        "//cc/bzd/container/range/views:all",
        "//cc/bzd/container/range/views:reverse",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility/bit",
    ],
)
