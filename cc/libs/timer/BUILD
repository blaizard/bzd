load("@bzd_rules_doc//doc:defs.bzl", "doc_cc_library")
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "timer_isr",
    hdrs = ["timer_isr.hh"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:non_owning_list",
        "//cc/bzd/core/async",
        "//cc/bzd/utility/synchronization:spin_mutex",
        "//cc/bzd/utility/synchronization:sync_lock_guard",
    ],
)

doc_cc_library(
    name = "doc",
    srcs = [
        "timer_isr.hh",
    ],
)
