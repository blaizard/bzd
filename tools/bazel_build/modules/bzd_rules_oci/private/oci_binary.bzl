"""Create a docker image from a binary target.

The image entry point, points to the binary.
"""

load("@bzd_bundle//:defs.bzl", "bzd_bundle_tar")
load("//:private/oci_image.bzl", "bzd_oci_image")
load("//:private/oci_load.bzl", "bzd_oci_load")

def bzd_oci_binary(name, binary, base = Label("@oci_minimal"), **kwargs):
    bzd_bundle_tar(
        name = "{}.package".format(name),
        output = "{}.package.tar".format(name),
        executables = {
            ".bzd_oci_binary_bootstrap": binary,
        },
    )

    bzd_oci_image(
        name = name,
        # Use entry point so that arguments can be passed with `command`.
        entrypoint = [
            "/.bzd_oci_binary_bootstrap",
        ],
        base = base,
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
