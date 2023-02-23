load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "timer",
    hdrs = ["timer.hh"],
    deps = [
        "//cc/bzd/container:non_owning_list",
        "//cc/bzd/utility/synchronization:spin_mutex",
        "//cc/bzd/utility/synchronization:sync_lock_guard",
    ],
)
