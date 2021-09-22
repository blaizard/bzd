load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "core",
    visibility = ["//visibility:public"],
    deps = [
        ":channel",
        ":delay",
        ":registry",
        ":units",
        "//cc/bzd/core/assert",
        "//cc/bzd/core/async",
        "//cc/bzd/core/logger",
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

cc_library(
    name = "channel",
    hdrs = [
        "channel.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:span",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
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
        "//cc/bzd/platform",
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
