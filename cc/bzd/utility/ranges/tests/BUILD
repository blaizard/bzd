load("@rules_cc//cc:defs.bzl", "cc_test")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "ranges",
    srcs = [
        "ranges.cc",
        "stream.cc",
        "subrange.cc",
        "views.cc",
    ],
    deps = [
        "//cc/bzd/algorithm:equal",
        "//cc/bzd/container",
        "//cc/bzd/test",
        "//cc/bzd/test/types:move_only",
        "//cc/bzd/test/types:range",
        "//cc/bzd/utility/ranges/views",
    ],
)

bzd_cc_test(
    name = "async",
    srcs = [
        "async.cc",
    ],
    deps = [
        "//cc/bzd/algorithm:equal",
        "//cc/bzd/container",
        "//cc/bzd/test",
        "//cc/bzd/test/types:generator",
        "//cc/bzd/test/types:ichannel",
        "//cc/bzd/utility/ranges/views_async",
    ],
)

cc_test(
    name = "build_no_async",
    srcs = [
        "build_no_async.cc",
    ],
    deps = [
        "//cc/bzd/utility/ranges/views",
    ],
)
