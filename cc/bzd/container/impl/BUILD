load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "impl",
    visibility = ["//visibility:public"],
    deps = [
        ":non_owning_list",
        ":single_linked_pool",
        ":span",
    ],
)

# ---- Individual items ----

cc_library(
    name = "non_owning_list",
    hdrs = [
        "non_owning_list.hh",
    ],
    visibility = ["//cc/bzd:__subpackages__"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/core/assert:minimal",
        "//cc/bzd/platform:atomic",
        "//cc/bzd/platform:types",
        "//cc/bzd/test:inject_point",
    ],
)

cc_library(
    name = "single_linked_pool",
    hdrs = [
        "single_linked_pool.hh",
    ],
    visibility = ["//cc/bzd/container:__pkg__"],
    deps = [
        "//cc/bzd/container:span",
        "//cc/bzd/core/assert",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "span",
    hdrs = [
        "span.hh",
    ],
    visibility = ["//cc/bzd/container:__pkg__"],
    deps = [
        "//cc/bzd/container/iterator:contiguous",
        "//cc/bzd/utility:forward",
    ],
)
