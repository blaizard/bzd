load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_binary")

cc_library(
    name = "lib",
    srcs = glob([
        "*.hh",
        "*.cc",
    ]),
    tags = ["manual"],
    deps = [
        "//cc:bzd",
        "//cc/libs/pthread",
    ],
)

bzd_cc_binary(
    name = "example",
    tags = ["manual"],
    deps = [
        ":lib",
    ],
)

bzd_package(
    name = "package",
    include_metadata = True,
    tags = ["manual"],
    deps = {
        ":example": "example",
    },
)

doc_library(
    name = "documentation",
    srcs = [
        ("System", "//example/system:documentation"),
    ],
    visibility = ["//visibility:public"],
)
