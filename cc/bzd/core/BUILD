load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "core",
    visibility = ["//visibility:public"],
    deps = [
        ":channel",
        ":clock",
        ":delay",
        ":error",
        ":print",
        ":units",
        "//cc/bzd/core/assert",
        "//cc/bzd/core/async",
        "//cc/bzd/core/logger",
        "//cc/bzd/core/panic",
    ],
)

# ---- Individual items ----

exports_files(
    ["logger.hh"],
    visibility = ["//cc/bzd/core/logger:__pkg__"],
)

exports_files(
    ["assert.hh"],
    visibility = ["//cc/bzd/core/assert:__pkg__"],
)

exports_files(
    ["async.hh"],
    visibility = ["//cc/bzd/core/async:__pkg__"],
)

exports_files(
    ["panic.hh"],
    visibility = ["//cc/bzd/core/panic:__pkg__"],
)

cc_library(
    name = "channel",
    hdrs = [
        "channel.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:span",
        "//cc/bzd/container:spans",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:distance",
        "//cc/bzd/utility/synchronization:lock_guard",
        "//cc/bzd/utility/synchronization:mutex",
    ],
)

cc_library(
    name = "delay",
    srcs = [
        "delay.cc",
    ],
    hdrs = [
        "delay.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":units",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:clock",
    ],
)

cc_library(
    name = "error",
    srcs = [
        "error.cc",
    ],
    hdrs = [
        "error.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container:pool",
        "//cc/bzd/container:result",
        "//cc/bzd/container:string",
        "//cc/bzd/container:variant",
        "//cc/bzd/type_traits:remove_cvref",
        "//cc/bzd/utility:source_location",
        "//cc/bzd/utility/string/to_string:format",
    ],
)

cc_library(
    name = "print",
    hdrs = [
        "print.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/platform:stream",
        "//cc/bzd/utility/string/to_stream:format",
    ],
)

cc_library(
    name = "units",
    hdrs = [
        "units.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:named_type",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "clock",
    hdrs = [
        "clock.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:named_type",
        "//cc/bzd/core:units",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
    ],
)
