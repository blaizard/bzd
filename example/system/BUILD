load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_system")
load("@bzd_bdl//diagram:defs.bzl", "bdl_system_diagram")
load("@rules_cc//cc:defs.bzl", "cc_library")

bdl_library(
    name = "composition",
    srcs = [
        "composition.bdl",
    ],
)

cc_library(
    name = "lib",
    hdrs = [
        "simple.hh",
    ],
    deps = [
        ":composition",
        "//cc:bzd",
    ],
)

bdl_system(
    name = "system",
    targets = {
        "esp32": "@@//cc/targets/esp32_idf:gcc",
        "hpc": "@@//cc/targets/linux:x86_64_clang",
    },
    deps = [
        ":lib",
    ],
)

bdl_system_diagram(
    name = "diagram",
    system = ":system",
)
