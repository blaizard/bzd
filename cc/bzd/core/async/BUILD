load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "async",
    hdrs = [
        "coroutine.hh",
        "executor.hh",
        "promise.hh",
        "//cc/bzd/core:async.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:function_view",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/container:tuple",
        "//cc/bzd/container/impl:non_owning_list",
        "//cc/bzd/core:error",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:ignore",
    ],
)
