load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "core",
    visibility = ["//visibility:public"],
    deps = [
        ":async",
        ":channel",
        ":delay",
        ":registry",
        ":units",
        "//cc/bzd/core/assert",
        "//cc/bzd/core/logger",
    ],
)

# ---- Individual items ----

exports_files(
    ["logger.h"],
    visibility = ["//cc/bzd/core/logger:__pkg__"],
)

exports_files(
    ["assert.h"],
    visibility = ["//cc/bzd/core/assert:__pkg__"],
)

cc_library(
    name = "channel",
    hdrs = [
        "channel.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:span",
        "//cc/bzd/core:async",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "delay",
    hdrs = [
        "delay.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":async",
        "//cc/bzd/platform",
    ],
)

cc_library(
    name = "async",
    hdrs = [
        "async.h",
        "coroutine.h",
        "executor.h",
        "promise.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:function_view",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/container:tuple",
        "//cc/bzd/container/impl:non_owning_list",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:ignore",
    ],
)

cc_library(
    name = "registry",
    hdrs = [
        "registry.h",
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
        "units.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:named_type",
        "//cc/bzd/platform:types",
    ],
)
