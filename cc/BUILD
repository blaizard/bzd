load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")
load("//tools/bazel_build/rules:documentation.bzl", "doc_library")

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

doc_library(
    name = "documentation",
    srcs = [
        ("Directory Layout", "README.md"),
        ("Core", "//cc/bzd/core/async:documentation"),
    ],
    visibility = ["//docs:__pkg__"],
)
