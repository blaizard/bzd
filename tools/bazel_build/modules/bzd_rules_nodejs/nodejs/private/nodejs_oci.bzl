"""Rules for NodeJs Docker."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")
load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_oci//:defs.bzl", "bzd_oci_image")

ROOT_DIRECTORY_ = "/bzd/bin"

def bzd_nodejs_oci(name, deps, cmd, base = "@docker//:nodejs", include_metadata = False, workdir = ROOT_DIRECTORY_, **kwargs):
    """Rule for embedding a NodeJs application into Docker.

    Args:
        name: The name of the target.
        deps: The dependencies.
        cmd: The command to be used.
        base: The base image.
        include_metadata: Whether metadata shall be included.
        workdir: The working directory.
        **kwargs: Extra arguments common to all build rules.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    bzd_package(
        name = "{}.package".format(name),
        tags = ["nodejs"],
        deps = {target: ROOT_DIRECTORY_ + "/" + dir_name for target, dir_name in deps.items()},
        include_metadata = include_metadata,
    )

    map_to_directory_ = {dir_name: (ROOT_DIRECTORY_ + "/" + dir_name) for dir_name in deps.values()}
    bzd_oci_image(
        name = name,
        base = base,
        cmd = [
            "node",
            "--experimental-json-modules",
        ] + [item.format(**map_to_directory_) for item in cmd],
        workdir = workdir.format(**map_to_directory_),
        env = {
            "NODE_ENV": select({
                "@bzd_lib//settings/build:dev": "development",
                "@bzd_lib//settings/build:prod": "production",
            }),
        },
        tags = ["nodejs"],
        tars = [
            "{}.package".format(name),
        ],
        **kwargs
    )
