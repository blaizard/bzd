load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_system", "bdl_target")
load("@bzd_bdl//diagram:defs.bzl", "bdl_system_diagram")
load("@bzd_rules_doc//doc:defs.bzl", "doc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//interfaces:io",
        "//interfaces:timer",
    ],
)

cc_library(
    name = "lib",
    srcs = [
        "jardinier.cc",
    ],
    hdrs = [
        "jardinier.hh",
    ],
    tags = [
        "no-clang-tidy",
    ],
    target_compatible_with = [
        "@bzd_platforms//al:esp_idf",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bdl_target(
    name = "board",
    composition = [
        ":board.bdl",
    ],
    parent = "@@//cc/targets/esp32s3_idf:gcc",
    deps = [
        "//cc/components/esp_idf/io/digital",
        "//cc/components/esp_idf/pwm",
    ],
)

bdl_system(
    name = "jardinier2000",
    targets = {
        "esp32": ":board",
    },
    deps = [
        ":lib",
    ],
)

bdl_system_diagram(
    name = "diagram",
    system = ":jardinier2000",
)

doc_binary(
    name = "documentation",
    srcs = [
        ("Overview", "README.md"),
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":diagram",
    ],
)
