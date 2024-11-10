"""Helper to support multi-platform with pip."""

load("@rules_python//python:pip.bzl", "pip_utils")

def requirement(name):
    """Helper to handle cross compilation."""

    _normalized_name = pip_utils.normalize_name(name)
    return select({
        Label("@bzd_platforms//al_isa:linux-x86_64"): ["@pip_linux_x86_64//{}".format(_normalized_name)],
        Label("@bzd_platforms//al_isa:linux-arm64"): ["@pip_linux_arm64//{}".format(_normalized_name)],
        "//conditions:default": [Label("@platforms//:incompatible")],
    })
