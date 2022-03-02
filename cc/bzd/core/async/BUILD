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
        "//cc/bzd/container:array",
        "//cc/bzd/container:function_ref",
        "//cc/bzd/container:non_owning_list",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/container:tuple",
        "//cc/bzd/container/range:associate_scope",
        "//cc/bzd/container/threadsafe:non_owning_forward_list",
        "//cc/bzd/core:error",
        "//cc/bzd/type_traits:is_base_of",
        "//cc/bzd/type_traits:is_same_template",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility:scope_guard",
        "//cc/bzd/utility/synchronization:spin_shared_mutex",
        "//cc/bzd/utility/synchronization:sync_lock_guard",
    ],
)
