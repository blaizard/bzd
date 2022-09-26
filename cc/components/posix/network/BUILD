load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
        "types.bdl",
    ],
    visibility = [
        "//visibility:public",
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
        ":interface",
        ":socket",
        "//cc/bzd/container:string",
        "//cc/bzd/container:string_view",
        "//cc/bzd/core:error",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:scope_guard",
        "//cc/bzd/utility/string/formatter:integral",
        "//cc/components/posix/network/address",
    ],
)

cc_library(
    name = "socket",
    srcs = [
        "socket.cc",
    ],
    hdrs = [
        "socket.hh",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/utility:to_underlying",
        "//cc/components/posix:error",
        "//cc/components/posix/io:file_descriptor",
        "//cc/components/posix/network/address",
    ],
)

cc_library(
    name = "socket_options",
    srcs = [
        "socket_options.cc",
    ],
    hdrs = [
        "socket_options.hh",
    ],
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:result",
        "//cc/components/posix:error",
        "//cc/components/posix/io:file_descriptor",
    ],
)
