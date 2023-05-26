load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "base",
    hdrs = [
        "base.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container/range:stream",
        "//cc/bzd/core/async",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:channel",
    ],
)

cc_library(
    name = "pattern",
    hdrs = [
        "pattern.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":base",
        "//cc/bzd/container:string_view",
        "//cc/bzd/utility/pattern:async",
        "//cc/bzd/utility/regexp:async",
    ],
)
