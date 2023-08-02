"""Module extension for CC toolchains."""

load("@bzd_platforms//:defs.bzl", "constraints_from_platform")
load("@bzd_toolchain_cc//:fragments/clang/defs.bzl", "clang")
load("@bzd_toolchain_cc//:fragments/esp32/defs.bzl", "esp32")
load("@bzd_toolchain_cc//:fragments/gcc/defs.bzl", "gcc")

_repositories = dict(**clang)
_repositories.update(esp32)
_repositories.update(gcc)

def _make_configs(name, version):
    configs = {}
    for target, config_by_executions in _repositories[version].items():
        for execution, factory in config_by_executions.items():
            configs["{}-{}-{}".format(name, execution, target)] = struct(
                factory = factory,
                execution = execution,
                target = target,
            )
    return configs

def _toolchain_repository_impl(repository_ctx):
    build_content = """# ---- Compiler constraint value
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

# ---- Toolchains (CC & Binary)
"""

    # Add the cc and binary toolchains.
    targets = {}
    configs = _make_configs(repository_ctx.attr.repo_name, repository_ctx.attr.version)
    for repo_name, config in configs.items():
        execution_constraints = constraints_from_platform(config.execution)
        target_constraints = constraints_from_platform(config.target)
        targets[config.target] = target_constraints

        build_content += """
# -------- {comment}

toolchain(
    name = "{name}",
    exec_compatible_with = [
        {execution_constraints}
    ],
    target_compatible_with = [
        ":toolchain",
        {target_constraints}
    ],
    toolchain = "@{repo_name}//:cc_toolchain",
    toolchain_type = "@rules_cc//cc:toolchain_type",
)

toolchain(
    name = "binary-{name}",
    exec_compatible_with = [
        {execution_constraints}
    ],
    target_compatible_with = [
        ":toolchain",
        {target_constraints}
    ],
    toolchain = "@{repo_name}//:binary_toolchain",
    toolchain_type = "@bzd_toolchain_cc//binary:toolchain_type",
)
""".format(
            comment = "{}/{} (execution/target)".format(config.execution, config.target),
            name = "{}-{}".format(config.execution, config.target),
            repo_name = repo_name,
            execution_constraints = "\n".join(["\"{}\",".format(c) for c in execution_constraints]),
            target_constraints = "\n".join(["\"{}\",".format(c) for c in target_constraints]),
        )

    build_content += "\n# ---- Platforms & Configs\n"

    # Add the various platforms for the targets.
    for target, target_constraints in targets.items():
        build_content += """
platform(
    name = "platform-{target}",
    constraint_values = [
        ":toolchain",
        {target_constraints}
    ],
    visibility = ["//visibility:public"],
)

config_setting(
    name = "{target}",
    constraint_values = [
        ":toolchain",
        {target_constraints}
    ],
    visibility = ["//visibility:public"],
)
""".format(
            target = target,
            target_constraints = "\n".join(["\"{}\",".format(c) for c in target_constraints]),
        )

    build_content += "\n# ---- Extra\n" + repository_ctx.attr.extra

    repository_ctx.file(
        "BUILD",
        executable = False,
        content = build_content,
    )

toolchain_repository = repository_rule(
    implementation = _toolchain_repository_impl,
    attrs = {
        "default": attr.bool(mandatory = True),
        "extra": attr.string(),
        "repo_name": attr.string(mandatory = True),
        "version": attr.string(values = _repositories.keys(), mandatory = True),
    },
)

def _toolchain_cc_impl(module_ctx):
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

    # Build the repositories.
    for name, toolchain in configs.items():
        # Create execution/target specific repositories.
        configs = _make_configs(name, toolchain.version)
        tools = {}
        for repo_name, config in configs.items():
            result = config.factory(module_ctx, repo_name)
            if result:
                for target, actual in result.setdefault("tools", {}).items():
                    tools.setdefault(target, {})["@bzd_platforms//al_isa:{}".format(config.execution)] = "@{}//:{}".format(repo_name, actual)

        extra = ""

        for tool_name, al_isa in tools.items():
            extra += """
alias(
    name = "{tool_name}",
    actual = select({{
        {select_al_isa}
    }}),
    visibility = ["//visibility:public"],
)
""".format(
                tool_name = tool_name,
                select_al_isa = ",\n".join(["\"{}\": \"{}\"".format(constraint, target) for constraint, target in al_isa.items()]),
            )

        # Create the main repository.
        toolchain_repository(
            name = name,
            repo_name = name,
            version = toolchain.version,
            default = toolchain.default,
            extra = extra,
        )

toolchain_cc = module_extension(
    implementation = _toolchain_cc_impl,
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
