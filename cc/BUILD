load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "bzd",
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm",
        "//cc/bzd/container",
        "//cc/bzd/core",
        "//cc/bzd/math",
        "//cc/bzd/meta",
        "//cc/bzd/type_traits",
        "//cc/bzd/utility",
    ],
)

filegroup(
    name = "documentation",
    srcs = glob([
        "bzd/**/*.hh",
    ]),
    visibility = ["//visibility:public"],
)
