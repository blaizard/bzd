load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "io",
    hdrs = [
        "buffer.hh",
        "sink.hh",
        "source.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:function_ref",
        "//cc/bzd/container:non_owning_list",
        "//cc/bzd/container:optional",
        "//cc/bzd/container/threadsafe:ring_buffer",
        "//cc/bzd/core/async",
        "//cc/bzd/meta:string_literal",
        "//cc/bzd/utility/synchronization:spin_mutex",
        "//cc/bzd/utility/synchronization:sync_lock_guard",
    ],
)
