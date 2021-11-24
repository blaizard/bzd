load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "core",
    visibility = ["//visibility:public"],
    deps = [
        ":channel",
        ":clock",
        ":delay",
        ":error",
        ":mutex",
        ":registry",
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
        ":mutex",
        "//cc/bzd/container:span",
        "//cc/bzd/container:spans",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:lock_guard",
    ],
)

cc_library(
    name = "delay",
    hdrs = [
        "delay.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
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
        "//cc/bzd/utility:source_location",
        "//cc/bzd/utility/format",
    ],
)

cc_library(
    name = "mutex",
    hdrs = [
        "mutex.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/platform:atomic",
    ],
)

cc_library(
    name = "registry",
    hdrs = [
        "registry.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:map",
        "//cc/bzd/container:result",
        "//cc/bzd/container:string_view",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:singleton",
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
        "//cc/bzd/platform:types",
    ],
)
