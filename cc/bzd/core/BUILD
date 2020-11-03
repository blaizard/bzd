load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "core",
    visibility = ["//visibility:public"],
    deps = [
        ":assert",
        ":channel",
        ":registry",
        ":system",
    ],
)

# ---- Individual items ----

cc_library(
    name = "assert_header",
    hdrs = [
        "assert.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "assert",
    srcs = [
        "assert.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":assert_header",
        ":channel",
        ":registry",
        ":system",
        "//cc/bzd/container:string_view",
    ],
)

cc_library(
    name = "channel",
    hdrs = [
        "channel.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/container:span",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "registry",
    hdrs = [
        "registry.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":assert_header",
        "//cc/bzd/container:map",
        "//cc/bzd/container:result",
        "//cc/bzd/container:string_view",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:singleton",
    ],
)

cc_library(
    name = "system_header",
    hdrs = [
        "system.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":channel",
    ],
)

cc_library(
    name = "system",
    srcs = [
        "system.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":registry",
        ":system_header",
        "//cc/bzd/lib/panic",
    ],
)
