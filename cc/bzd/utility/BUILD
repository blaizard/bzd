load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "utility",
    visibility = ["//visibility:public"],
    deps = [
        ":aligned_storage",
        ":bit",
        ":container_of",
        ":forward",
        ":max",
        ":min",
        ":move",
        ":offset_of",
        ":signal",
        ":singleton",
        ":swap",
    ],
)

# ---- Individual items ----

cc_library(
    name = "aligned_storage",
    hdrs = [
        "aligned_storage.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "bit",
    hdrs = [
        "bit.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "container_of",
    hdrs = [
        "container_of.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":offset_of",
    ],
)

cc_library(
    name = "forward",
    hdrs = [
        "forward.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:is_lvalue_reference",
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "max",
    hdrs = [
        "max.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "min",
    hdrs = [
        "min.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "move",
    hdrs = [
        "move.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/type_traits:remove_reference",
    ],
)

cc_library(
    name = "offset_of",
    hdrs = [
        "offset_of.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "signal",
    hdrs = [
        "signal.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm:copy_n",
        "//cc/bzd/algorithm:reverse",
        "//cc/bzd/container:buffer",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "singleton",
    hdrs = [
        "singleton.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "swap",
    hdrs = [
        "swap.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":move",
    ],
)
