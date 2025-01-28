load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_system")
load("@bzd_bdl//diagram:defs.bzl", "bdl_system_diagram")
load("@bzd_rules_doc//doc:defs.bzl", "doc_binary")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
    deps = [
        "//cc/components/esp_idf/pwm",
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
    target_compatible_with = [
        "@bzd_platforms//al:esp_idf",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bdl_system(
    name = "jardinier2000",
    targets = {
        "esp32": "@@//cc/targets/esp32_idf:gcc",
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
    data = [
        ":diagram",
    ],
    visibility = ["//visibility:public"],
)
