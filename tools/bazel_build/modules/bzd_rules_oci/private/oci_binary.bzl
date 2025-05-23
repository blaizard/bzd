"""Create a docker image from a binary target."""

load("@bzd_bundle//:defs.bzl", "bzd_bundle_tar")
load("//:private/oci_image.bzl", "bzd_oci_image")

ROOT_DIRECTORY_ = "/bzd/bin"

def bzd_oci_binary(name, binary, **kwargs):
    bzd_bundle_tar(
        name = "{}.package".format(name),
        output = "{}.package.tar".format(name),
        executables = {
            ROOT_DIRECTORY_: binary,
        },
        bootstrap_script = ".bzd_oci_binary_bootstrap",
    )

    bzd_oci_image(
        name = name,
        # Use entry point so that arguments can be passed with `command`.
        entrypoint = [
            "{}/.bzd_oci_binary_bootstrap".format(ROOT_DIRECTORY_),
        ],
        tars = [
            "{}.package.tar".format(name),
        ],
        **kwargs
    )
