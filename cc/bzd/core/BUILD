load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "core",
    visibility = ["//visibility:public"],
    deps = [
        ":assert",
        ":channel",
        ":log",
        ":promise",
        ":registry",
        ":scheduler",
        ":task",
    ],
)

# ---- Individual items ----

cc_library(
    name = "assert",
    hdrs = [
        "assert.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":log",
        "//cc/bzd/container:result",
        "//cc/bzd/core:assert_minimal",
        "//cc/bzd/platform:system",
    ],
)

cc_library(
    name = "assert_minimal_header",
    hdrs = [
        "assert/minimal.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility:source_location",
    ],
)

cc_library(
    name = "assert_minimal",
    srcs = [
        "assert/minimal.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":assert_minimal_header",
        ":channel",
        ":registry",
        "//cc/bzd/container:string_view",
        "//cc/bzd/platform:system",
        "//cc/bzd/utility/format:integral",
    ],
)

cc_library(
    name = "channel",
    hdrs = [
        "channel.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/container:span",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "log",
    hdrs = [
        "log.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":channel",
        "//cc/bzd/container:string",
        "//cc/bzd/container:string_view",
        "//cc/bzd/platform:system",
        "//cc/bzd/utility:forward",
        "//cc/bzd/utility/format",
    ],
)

cc_library(
    name = "promise",
    hdrs = [
        "promise.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/container:optional",
    ],
)

cc_library(
    name = "registry",
    hdrs = [
        "registry.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":assert_minimal_header",
        "//cc/bzd/container:map",
        "//cc/bzd/container:result",
        "//cc/bzd/container:string_view",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:singleton",
    ],
)

cc_library(
    name = "scheduler",
    srcs = [
        "scheduler.cc",
    ],
    hdrs = [
        "scheduler.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":task",
        ":promise",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:queue",
        "//cc/bzd/utility:move",
        "//cc/bzd/utility:singleton",
        "//cc/bzd/utility:swap",
    ],
)

cc_library(
    name = "task",
    hdrs = [
        "task.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":assert",
        "//cc/bzd/container:function",
        "//cc/bzd/platform:system",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:forward",
    ],
)
