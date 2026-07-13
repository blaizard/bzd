"""Create a docker image from a nodejs binary target."""

load("@bzd_lib//:defs.bzl", "bzd_filegroup_no_runfiles")
load("@bzd_rules_oci//:defs.bzl", "bzd_oci_image_from_binary")

def bzd_nodejs_oci_binary(name, binary, **kwargs):
    bzd_filegroup_no_runfiles(
        name = "{}.node_modules".format(name),
        srcs = [
            binary,
        ],
        output_group = "node_modules",
        tags = ["manual"],
    )

    bzd_oci_image_from_binary(
        name = name,
        binary = binary,
        layers = [
            "{}.node_modules".format(name),
        ],
        **kwargs
    )
