"""Helper test rules for bdl system."""

load("@bzd_lib//:defs.bzl", "bzd_diff_test")
load("//:defs.bzl", "bdl_system")

def bzl_system_json(name, deps):
    bdl_system(
        name = "{}.system".format(name),
        targets = {
            "default": Label(":target"),
        },
        deps = deps,
    )

    native.alias(
        name = name,
        actual = "{}.system.default".format(name),
    )

def bzl_system_test(name, deps, expected_json):
    bzl_system_json(
        name = "{}.json".format(name),
        deps = deps,
    )

    native.filegroup(
        name = "{}.file".format(name),
        srcs = [
            "{}.json".format(name),
        ],
    )

    bzd_diff_test(
        name = name,
        format = "json",
        file1 = "{}.file".format(name),
        file2 = expected_json,
    )
