load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "synchronization",
    visibility = ["//visibility:public"],
    deps = [
        ":lock_guard",
        ":mutex",
        ":spin_mutex",
        ":spin_shared_mutex",
        ":sync_lock_guard",
    ],
)

# ---- Individual items ----

cc_library(
    name = "lock_guard",
    hdrs = [
        "lock_guard.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/type_traits:invoke_result",
        "//cc/bzd/type_traits:lockable",
        "//cc/bzd/utility:scope_guard",
    ],
)

cc_library(
    name = "mutex",
    hdrs = [
        "mutex.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core/async",
        "//cc/bzd/platform:atomic",
    ],
)

cc_library(
    name = "spin_mutex",
    hdrs = [
        "spin_mutex.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:atomic",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "spin_shared_mutex",
    hdrs = [
        "spin_shared_mutex.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:atomic",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "sync_lock_guard",
    hdrs = [
        "sync_lock_guard.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:lockable",
        "//cc/bzd/utility:scope_guard",
    ],
)
