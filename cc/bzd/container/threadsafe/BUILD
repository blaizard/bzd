load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "threadsafe",
    visibility = ["//visibility:public"],
    deps = [
        ":non_owning_forward_list",
    ],
)

# ---- Individual items ----

cc_library(
    name = "non_owning_forward_list",
    hdrs = [
        "non_owning_forward_list.hh",
    ],
    visibility = ["//cc/bzd:__subpackages__"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container:reference_wrapper",
        "//cc/bzd/container:result",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:atomic",
        "//cc/bzd/platform:types",
        "//cc/bzd/test:inject_point",
        "//cc/bzd/type_traits:add_const",
        "//cc/bzd/type_traits:is_const",
        "//cc/bzd/type_traits:iterator",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility:scope_guard",
    ],
)

cc_library(
    name = "non_owning_queue2",
    hdrs = [
        "non_owning_queue.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/platform:atomic",
        "//cc/bzd/test:inject_point",
        "//cc/bzd/utility:ignore",
    ],
)

cc_library(
    name = "non_owning_queue_spin",
    hdrs = [
        "non_owning_queue_spin.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/utility:ignore",
        "//cc/bzd/utility/synchronization:spin_mutex",
        "//cc/bzd/utility/synchronization:sync_lock_guard",
    ],
)
