load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_system")
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
        "esp32": "@@//cc/targets/esp32_idf_xtensa_lx6:gcc",
        "hpc": "@@//cc/targets/linux:x86_64_clang",
    },
    deps = [
        ":lib",
    ],
)
