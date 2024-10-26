"""Module extension for NodeJs toolchains."""

load("@bzd_lib//:repository_maker.bzl", "repository_maker")
load("@bzd_rules_nodejs//toolchain/node:defs.bzl", "node_install", "node_versions")
load("@bzd_rules_nodejs//toolchain/pnpm:defs.bzl", "pnpm_install")

def _toolchain_repository_build_content(node, pnpm, default):
    build_content = """load("@bzd_rules_nodejs//nodejs:toolchain.bzl", "nodejs_toolchain")
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

# ---- Compiler constraint value
"""

    # Generate the constraint value for the toolchain.
    if default:
        build_content += """
alias(
    name = "toolchain",
    actual = "@bzd_platforms//toolchain:default",
    visibility = ["//visibility:public"],
)
"""
    else:
        build_content += """
constraint_value(
    name = "toolchain",
    constraint_setting = "@bzd_platforms//toolchain:toolchain",
    visibility = ["//visibility:public"],
)
"""

    build_content += """

platform(
    name = "platform",
    parents = [
        "@local_config_platform//:host",
    ],
    constraint_values = [
        ":toolchain",
    ],
    visibility = ["//visibility:public"],
)

# ---- Toolchains

sh_binary_wrapper(
    name = "node",
    binary = "{node}",
    command = "{{binary}} --preserve-symlinks --preserve-symlinks-main --use-strict $@",
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "pnpm",
    locations = {{
        "{pnpm}": "binary",
        "{node}": "node_binary"
    }},
    command = "PATH=$(dirname {{node_binary}}):$PATH {{binary}} --shamefully-hoist --store-dir=/tmp/pnpm --color $@",
    data = [
        "{node}",
    ],
    visibility = ["//visibility:public"],
)

nodejs_toolchain(
    name = "nodejs_toolchain_specific",
    manager = ":pnpm",
    node = ":node",
)

toolchain(
    name = "nodejs_toolchain",
    target_compatible_with = [
        ":toolchain",
    ],
    toolchain = ":nodejs_toolchain_specific",
    toolchain_type = "@bzd_rules_nodejs//nodejs:toolchain_type",
)
""".format(
        node = node,
        pnpm = pnpm,
    )

    return build_content

def _toolchain_nodejs_impl(module_ctx):
    # Gather all the toolchains registered.
    configs = {}
    for mod in module_ctx.modules:
        for toolchain in mod.tags.toolchain:
            # No need to check the version, it is already enforced in the attribute.
            if toolchain.name in configs:
                fail("A toolchain with the name '{}' already exists.".format(toolchain.name))
            configs[toolchain.name] = struct(
                version = toolchain.version,
                default = toolchain.default,
            )

    # Add the node repositories.
    for name, toolchain in configs.items():
        node_install(
            name = "node-" + toolchain.version,
            version = toolchain.version,
        )

        # Create the main repository.
        repository_maker(
            name = name,
            create = {
                "BUILD": _toolchain_repository_build_content(
                    node = "@node-" + toolchain.version + "//:node",
                    pnpm = "@pnpm//:pnpm",
                    default = toolchain.default,
                ),
            },
        )

    # Install the pnpm repository
    pnpm_install(
        name = "pnpm",
    )

toolchain_nodejs = module_extension(
    implementation = _toolchain_nodejs_impl,
    tag_classes = {
        "toolchain": tag_class(
            attrs = {
                "default": attr.bool(default = False),
                "name": attr.string(mandatory = True),
                "version": attr.string(values = node_versions, mandatory = True),
            },
        ),
    },
)
