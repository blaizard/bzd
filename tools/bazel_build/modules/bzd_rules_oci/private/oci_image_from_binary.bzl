"""Create a docker image from a binary target.

The image entry point, points to the binary.
"""

load("@bzd_bundle//:defs.bzl", "bzd_bundle_tar")
load("//:private/oci_image.bzl", "bzd_oci_image")

def bzd_oci_image_from_binary(name, binary, base = Label("@oci_minimal"), layers = None, **kwargs):
    bzd_bundle_tar(
        name = "{}.package".format(name),
        output = "{}.package.tgz".format(name),
        compression = "gz",
        root_symlinks = {
            ".bzd_oci_binary_bootstrap": binary,
        },
        srcs = [binary],
        include_runfiles = True,
        layers = layers or [],
        tags = ["manual"],
    )

    bzd_oci_image(
        name = name,
        # Use entrypoint to that argments can be passed with `command`.
        entrypoint = [
            "/.bzd_oci_binary_bootstrap",
        ],
        tars = [
            "{}.package.tar".format(name),
        ],
        base = base,
        **kwargs
    )
