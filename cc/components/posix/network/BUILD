load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
)

cc_library(
    name = "network",
    hdrs = [
        "network.hh",
    ],
    target_compatible_with = select({
        "@esp32_xtensa_lx6_gcc//:compiler": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/components/posix/network/socket:ipv4",
        "//cc/bzd/container:string",
        "//cc/bzd/container:string_view",
        "//cc/bzd/core:error",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:scope_guard",
        "//cc/bzd/utility/format:integral",
    ],
)
