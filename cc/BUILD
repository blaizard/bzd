load("@bzd_rules_doc//doc:defs.bzl", "doc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_library")

bzd_cc_library(
    name = "bzd",
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/algorithm",
        "//cc/bzd/container",
        "//cc/bzd/core",
        "//cc/bzd/math",
        "//cc/bzd/meta",
        "//cc/bzd/platform",
        "//cc/bzd/type_traits",
        "//cc/bzd/utility",
    ],
)

doc_library(
    name = "documentation",
    srcs = [
        ("Directory Layout", "README.md"),
        ("**", "//cc/bzd/core/async:documentation"),
        ("**", "//cc/bzd/utility/pattern:documentation"),
    ],
    visibility = ["//docs:__pkg__"],
)
