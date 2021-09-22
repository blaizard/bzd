load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "async",
    hdrs = [
        "coroutine.h",
        "executor.h",
        "promise.h",
        "//cc/bzd/core:async.h",
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
