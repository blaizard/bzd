"""Module extension for NodeJs toolchains."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bzd_platforms//:defs.bzl", "constraints_from_platform")
load("@bzd_rules_nodejs//toolchain/pnpm:defs.bzl", "pnpm_install")

_repositories = {
    "node_20.11.0": {
        "linux-x86_64": {
            "build_file": "@bzd_rules_nodejs//nodejs:linux_x86_64.BUILD",
            "integrity": "sha256-gieANp0Oownn0hjkHeu9GgP4zfNU6/ikQg6J85zC5hI=",
            "strip_prefix": "node-v20.11.0-linux-x64",
            "urls": [
                "https://nodejs.org/dist/v20.11.0/node-v20.11.0-linux-x64.tar.xz",
            ],
        },
    },
    "node_20.5.1": {
        "linux-x86_64": {
            "build_file": "@bzd_rules_nodejs//nodejs:linux_x86_64.BUILD",
            "sha256": "a4a700bbca51ac26538eda2250e449955a9cc49638a45b38d5501e97f5b020b4",
            "strip_prefix": "node-v20.5.1-linux-x64",
            "urls": [
                "https://nodejs.org/dist/v20.5.1/node-v20.5.1-linux-x64.tar.xz",
            ],
        },
    },
}

def _make_configs(name, version):
    configs = {}
    for execution, fetch_info in _repositories[version].items():
        configs["{}-{}".format(name, execution)] = struct(
            fetch = fetch_info,
            execution = execution,
        )
    return configs

def _toolchain_repository_impl(repository_ctx):
    build_content = """load("@bzd_rules_nodejs//nodejs:toolchain.bzl", "nodejs_toolchain")
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper")

# ---- Compiler constraint value
"""

    # Generate the constraint value for the toolchain.
    if repository_ctx.attr.default:
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
"""

    # Add the nodejs toolchains.
    configs = _make_configs(repository_ctx.attr.repo_name, repository_ctx.attr.version)
    for repo_name, config in configs.items():
        execution_constraints = constraints_from_platform(config.execution)

        build_content += """
# -------- {comment}

sh_binary_wrapper(
    name = "{execution}_node",
    binary = "@{repo_name}//:node",
    command = "{{binary}} --preserve-symlinks --preserve-symlinks-main --use-strict $@",
    visibility = ["//visibility:public"],
)

sh_binary_wrapper(
    name = "{execution}_pnpm",
    locations = {{
        "@pnpm//:pnpm": "binary",
        "@{repo_name}//:node": "node"
    }},
    command = "PATH=$(dirname {{node}}):$PATH {{binary}} --color $@",
    data = [
        "@{repo_name}//:node",
    ],
    visibility = ["//visibility:public"],
)

nodejs_toolchain(
    name = "{name}_nodejs_toolchain",
    manager = ":{execution}_pnpm",
    node = ":{execution}_node",
)

toolchain(
    name = "{name}",
    exec_compatible_with = [
        {execution_constraints}
    ],
    target_compatible_with = [
        ":toolchain",
        {execution_constraints}
    ],
    toolchain = ":{name}_nodejs_toolchain",
    toolchain_type = "@bzd_rules_nodejs//nodejs:toolchain_type",
)
""".format(
            comment = config.execution,
            name = config.execution,
            repo_name = repo_name,
            execution = config.execution,
            execution_constraints = "\n".join(["\"{}\",".format(c) for c in execution_constraints]),
        )

    repository_ctx.file(
        "BUILD",
        executable = False,
        content = build_content,
    )

toolchain_repository = repository_rule(
    implementation = _toolchain_repository_impl,
    attrs = {
        "default": attr.bool(mandatory = True),
        "repo_name": attr.string(mandatory = True),
        "version": attr.string(values = _repositories.keys(), mandatory = True),
    },
)

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
        # Create execution specific repositories.
        configs = _make_configs(name, toolchain.version)
        for repo_name, config in configs.items():
            http_archive(
                name = repo_name,
                **config.fetch
            )

        # Create the main repository.
        toolchain_repository(
            name = name,
            repo_name = name,
            version = toolchain.version,
            default = toolchain.default,
        )

    # Install the pnpm repository
    pnpm_install()

toolchain_nodejs = module_extension(
    implementation = _toolchain_nodejs_impl,
    tag_classes = {
        "toolchain": tag_class(
            attrs = {
                "default": attr.bool(default = False),
                "name": attr.string(mandatory = True),
                "version": attr.string(values = _repositories.keys(), mandatory = True),
            },
        ),
    },
)
