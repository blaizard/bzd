load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "impl",
    visibility = ["//visibility:public"],
    deps = [
        ":non_owning_list",
        ":single_linked_pool",
    ],
)

# ---- Individual items ----

cc_library(
    name = "non_owning_list",
    hdrs = [
        "non_owning_list.h",
    ],
    visibility = ["//cc/bzd:__subpackages__"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/core:assert",
        "//cc/bzd/platform:atomic",
        "//cc/bzd/platform:types",
        "//cc/bzd/test:inject_point",
    ],
)

cc_library(
    name = "single_linked_pool",
    hdrs = [
        "single_linked_pool.h",
    ],
    visibility = ["//cc/bzd/container:__pkg__"],
    deps = [
        "//cc/bzd/container:span",
        "//cc/bzd/core:assert_minimal",
        "//cc/bzd/platform:types",
    ],
)
