load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "integral",
    hdrs = [
        "integral.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string_view",
        "//cc/bzd/type_traits:is_integral",
        "//cc/bzd/type_traits:is_signed",
        "//cc/bzd/utility:begin",
        "//cc/bzd/utility:end",
    ],
)

cc_library(
    name = "from_string",
    hdrs = [
        "from_string.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":integral",
        "//cc/bzd/type_traits:container",
        "//cc/bzd/utility/string:base",
    ],
)
