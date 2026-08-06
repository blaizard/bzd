"""C++ extension for bdl rules."""

load("@rules_cc//cc:defs.bzl", "CcInfo")
load("//cc/bdl/generator:composition.bzl", "generator_cc_composition")
load("//cc/bdl/generator:library.bzl", "generator_cc_library")

extension = {
    "composition": {
        "generator": generator_cc_composition,
    },
    "library": {
        "generator": generator_cc_library,
        "providers": [CcInfo],
    },
}
