"""Rules for NodeJs Docker."""

load("@bzd_lib//lib:attrs.bzl", "ATTRS_COMMON_BUILD_RULES", "attrs_assert_any_of")
load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_docker//:defs.bzl", "bzd_docker_image", "bzd_docker_push")

ROOT_DIRECTORY_ = "/bzd/bin"

def bzd_nodejs_docker(name, deps, cmd, base = "@docker//:nodejs", include_metadata = False, deploy = {}, **kwargs):
    """Rule for embedding a NodeJs application into Docker.

    Args:
        name: The name of the target.
        deps: The dependencies.
        cmd: The command to be used.
        base: The base image.
        include_metadata: Wether metadata shall be included.
        deploy: The deploy rules.
        **kwargs: Extra arguments common to all build rules.
    """

    attrs_assert_any_of(kwargs, ATTRS_COMMON_BUILD_RULES)

    bzd_package(
        name = "{}.package".format(name),
        tags = ["nodejs"],
        deps = {target: ROOT_DIRECTORY_ + "/" + dir_name for target, dir_name in deps.items()},
        include_metadata = include_metadata,
    )

    map_to_directory_ = {dir_name: dir_name for dir_name in deps.values()}
    bzd_docker_image(
        name = name,
        base = base,
        cmd = [
            "node",
            "--experimental-json-modules",
        ] + [item.format(**map_to_directory_) for item in cmd],
        workdir = ROOT_DIRECTORY_,
        env = {
            "NODE_ENV": "production",
        },
        tags = ["nodejs"],
        tars = [
            "{}.package".format(name),
        ],
        **kwargs
    )

    # Add deploy rules if any
    for rule_name, url in deploy.items():
        bzd_docker_push(
            name = rule_name,
            image = ":{}".format(name),
            repository = url,
            remote_tags = ["latest"],
            tags = [
                "docker",
            ],
        )
