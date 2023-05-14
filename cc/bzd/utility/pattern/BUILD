load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:documentation.bzl", "doc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "to_string",
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility/pattern/to_string:boolean",
        "//cc/bzd/utility/pattern/to_string:floating_point",
        "//cc/bzd/utility/pattern/to_string:integral",
        "//cc/bzd/utility/pattern/to_string:pattern",
        "//cc/bzd/utility/pattern/to_string:pointer",
        "//cc/bzd/utility/pattern/to_string:string",
    ],
)

bzd_cc_library(
    name = "to_stream",
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility/pattern/to_stream:boolean",
        "//cc/bzd/utility/pattern/to_stream:floating_point",
        "//cc/bzd/utility/pattern/to_stream:integral",
        "//cc/bzd/utility/pattern/to_stream:pattern",
        "//cc/bzd/utility/pattern/to_stream:pointer",
        "//cc/bzd/utility/pattern/to_stream:string",
    ],
)

bzd_cc_library(
    name = "from_string",
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility/pattern/from_string:integral",
        "//cc/bzd/utility/pattern/from_string:output_range",
        "//cc/bzd/utility/pattern/from_string:pattern",
    ],
)

cc_library(
    name = "pattern",
    hdrs = [
        "pattern.hh",
    ],
    visibility = [
        ":__subpackages__",
    ],
    deps = [
        "//cc/bzd/container:function_ref",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:string_view",
        "//cc/bzd/container:tuple",
        "//cc/bzd/container:vector",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/utility:apply",
        "//cc/bzd/utility:constexpr_for",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility/pattern/from_string:integral",
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

doc_library(
    name = "documentation",
    srcs = [
        ("Pattern", "README.md"),
    ],
    visibility = ["//cc:__pkg__"],
)
