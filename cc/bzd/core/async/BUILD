load("@rules_cc//cc:defs.bzl", "cc_library")
load("@bzd_rules_doc//doc:defs.bzl", "doc_library")

doc_library(
    name = "documentation",
    srcs = [
        ("Async", "README.md"),
    ],
    visibility = ["//cc:__pkg__"],
)

cc_library(
    name = "async",
    hdrs = [
        "awaitables/default.hh",
        "awaitables/enqueue.hh",
        "awaitables/get_executable.hh",
        "awaitables/get_executor.hh",
        "awaitables/propagate.hh",
        "awaitables/suspend.hh",
        "awaitables/yield.hh",
        "cancellation.hh",
        "coroutine.hh",
        "executable.hh",
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
        "//cc/bzd/container:variant",
        "//cc/bzd/container/ranges:associate_scope",
        "//cc/bzd/container/threadsafe:bitset",
        "//cc/bzd/container/threadsafe:non_owning_forward_list",
        "//cc/bzd/container/threadsafe:non_owning_ring_spin",
        "//cc/bzd/core:error",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/meta:always_false",
        "//cc/bzd/type_traits:is_base_of",
        "//cc/bzd/type_traits:is_lvalue_reference",
        "//cc/bzd/type_traits:is_same_template",
        "//cc/bzd/type_traits:remove_reference",
        "//cc/bzd/utility:apply",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility:scope_guard",
        "//cc/bzd/utility/synchronization:spin_mutex",
        "//cc/bzd/utility/synchronization:spin_shared_mutex",
        "//cc/bzd/utility/synchronization:sync_lock_guard",
    ],
)
