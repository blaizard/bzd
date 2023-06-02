load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_library")

package(default_testonly = True)

bzd_cc_library(
    name = "types",
    visibility = ["//visibility:public"],
    deps = [
        ":copy_only",
        ":ichannel",
        ":istream_reader",
        ":lifetime_counter",
        ":move_only",
        ":no_default_constructor",
        ":range",
    ],
)

# ---- Individual items ----

cc_library(
    name = "copy_only",
    hdrs = [
        "copy_only.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "ichannel",
    hdrs = [
        "ichannel.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:ring_buffer",
        "//cc/bzd/container:span",
        "//cc/bzd/container:string_view",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:to_underlying",
    ],
)

cc_library(
    name = "istream_reader",
    hdrs = [
        "istream_reader.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:string",
        "//cc/bzd/core:channel",
        "//cc/bzd/core/async",
        "//cc/bzd/platform:types",
    ],
)

cc_library(
    name = "lifetime_counter",
    hdrs = [
        "lifetime_counter.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "move_only",
    hdrs = [
        "move_only.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:types",
        "//cc/bzd/utility:move",
    ],
)

cc_library(
    name = "no_default_constructor",
    hdrs = [
        "no_default_constructor.hh",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "range",
    hdrs = [
        "range.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/container:array",
        "//cc/bzd/type_traits:range",
    ],
)
