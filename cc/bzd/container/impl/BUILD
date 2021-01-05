load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "impl",
    visibility = ["//visibility:public"],
    deps = [
        ":single_linked_list",
        ":single_linked_pool",
    ],
)

# ---- Individual items ----

cc_library(
    name = "single_linked_list",
    hdrs = [
        "single_linked_list.h",
    ],
    visibility = ["//cc/bzd:__subpackages__"],
    deps = [
        "//cc/bzd/container:optional",
        "//cc/bzd/container:result",
        "//cc/bzd/core:assert_minimal",
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
