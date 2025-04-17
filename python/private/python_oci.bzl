"""Rules for NodeJs Docker."""

load("@bzd_rules_oci//:defs.bzl", "bzd_oci_binary", "bzd_oci_load")
load("//private:python_hermetic_binary.bzl", "bzd_python_hermetic_launcher")

def bzd_python_oci(name, binary, base = "@docker//:minimal", **kwargs):
    """Rule for embedding a python application into Docker.

    Args:
        name: The name of the target.
        binary: The python binary.
        base: The base image.
        **kwargs: Extra arguments common to all build rules.
    """

    bzd_python_hermetic_launcher(
        name = "{}.hermetic".format(name),
        binary = binary,
    )

    bzd_oci_binary(
        name = name,
        binary = "{}.hermetic".format(name),
        base = base,
        **kwargs
    )

    bzd_oci_load(
        name = "{}.load".format(name),
        image = name,
        remote_tags = ["latest"],
        repository = "local/{}".format(name),
    )
