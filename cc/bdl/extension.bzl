"""C++ extension for bdl rules."""

load("@rules_cc//cc:defs.bzl", "CcInfo")
load("//cc/bdl/generator:composition.bzl", "generator_cc_composition")
load("//cc/bdl/generator:library.bzl", "generator_cc_library")

def _get_cc_public_header(target):
    """Get all the direct public headers from a target."""

    if CcInfo not in target:
        return []
    return target[CcInfo].compilation_context.direct_public_headers

def _aspect_files(target):
    return _get_cc_public_header(target)

extension = {
    "aspect_files": {
        "hdrs": _aspect_files,
    },
    "composition": {
        "generator": generator_cc_composition,
    },
    "library": {
        "generator": generator_cc_library,
        "providers": [CcInfo],
    },
}
