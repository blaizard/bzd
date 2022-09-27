load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "pattern",
    hdrs = [
        "pattern.hh",
    ],
    visibility = [
        ":__subpackages__",
    ],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string_view",
        "//cc/bzd/container:tuple",
        "//cc/bzd/container:vector",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/meta:tuple",
        "//cc/bzd/utility:constexpr_for",
    ],
)

cc_library(
    name = "async",
    hdrs = [
        "async.hh",
    ],
    visibility = [
        ":__subpackages__",
    ],
    deps = [
        ":pattern",
        "//cc/bzd/core/async",
    ],
)
