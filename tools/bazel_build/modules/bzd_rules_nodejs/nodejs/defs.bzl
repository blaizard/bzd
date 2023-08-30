"""Rules for NodeJs."""

load("@bzd_package//:defs.bzl", "bzd_package")
load("@bzd_rules_nodejs//nodejs:private/nodejs_executable.bzl", bzd_nodejs_binary_ = "bzd_nodejs_binary", bzd_nodejs_test_ = "bzd_nodejs_test")
load("@bzd_rules_nodejs//nodejs:private/nodejs_extern_binary.bzl", bzd_nodejs_extern_binary_ = "bzd_nodejs_extern_binary")
load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", bzd_nodejs_install_ = "bzd_nodejs_install")
load("@bzd_rules_nodejs//nodejs:private/nodejs_library.bzl", bzd_nodejs_library_ = "bzd_nodejs_library")
load("@bzd_rules_nodejs//nodejs:private/nodejs_web_binary.bzl", bzd_nodejs_web_binary_ = "bzd_nodejs_web_binary")
load("@rules_oci//oci:defs.bzl", "oci_image", "oci_push")

# Public API.

bzd_nodejs_install = bzd_nodejs_install_
bzd_nodejs_library = bzd_nodejs_library_
bzd_nodejs_extern_binary = bzd_nodejs_extern_binary_
bzd_nodejs_web_binary = bzd_nodejs_web_binary_

def bzd_nodejs_binary(name, main, args = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs", "manual"],
        **kwargs
    )

    bzd_nodejs_binary_(
        name = name,
        main = main,
        args = args,
        install = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )

def bzd_nodejs_test(name, main, args = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs", "manual"],
        **kwargs
    )

    bzd_nodejs_test_(
        name = name,
        main = main,
        args = args,
        executor = "@bzd_rules_nodejs//toolchain/mocha",
        install = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )

# ---- Docker Package ----

def bzd_nodejs_docker(name, deps, cmd, base = "@docker//:nodejs", include_metadata = False, deploy = {}):
    """Rule for embedding a NodeJs application into Docker.

    Args:
        name: The name of the target.
        deps: The dependencies.
        cmd: The command to be used.
        base: The base image.
        include_metadata: Wether metadata shall be included.
        deploy: The deploy rules.
    """

    bzd_package(
        name = "{}.package".format(name),
        tags = ["nodejs"],
        deps = deps,
        include_metadata = include_metadata,
    )

    root_directory = "/bzd/bin"
    map_to_directory = {dir_name: "{}/{}".format(root_directory, dir_name) for dir_name in deps.values()}

    oci_image(
        name = name,
        base = base,
        cmd = [
            "node",
            "--experimental-json-modules",
        ] + [item.format(**map_to_directory) for item in cmd],
        workdir = root_directory,
        env = {
            "NODE_ENV": "production",
        },
        #stamp = "@io_bazel_rules_docker//stamp:always",
        tags = ["nodejs"],
        tars = [
            "{}.package".format(name),
        ],
    )

    # Add deploy rules if any
    for rule_name, url in deploy.items():
        oci_push(
            name = rule_name,
            image = ":{}".format(name),
            repository = url,
            remote_tags = ["latest"],
            tags = [
                "docker",
            ],
        )
