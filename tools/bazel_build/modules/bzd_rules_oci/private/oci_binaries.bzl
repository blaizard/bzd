"""Create a docker image from multiple binary targets.

This rule does not overwrite the entry point.
"""

load("@bzd_bundle//:defs.bzl", "bzd_bundle_tar")
load("//:private/oci_image.bzl", "bzd_oci_image")
load("//:private/oci_load.bzl", "bzd_oci_load")

def bzd_oci_binaries(name, binaries, **kwargs):
    bzd_bundle_tar(
        name = "{}.package".format(name),
        output = "{}.package.tar".format(name),
        executables = binaries,
    )

    bzd_oci_image(
        name = name,
        tars = [
            "{}.package.tar".format(name),
        ],
        **kwargs
    )

    # Helper to load the image locally.
    bzd_oci_load(
        name = "{}.load".format(name),
        image = name,
        remote_tags = ["latest"],
        repository = "local/{}".format(name),
        tags = ["manual"],
    )
