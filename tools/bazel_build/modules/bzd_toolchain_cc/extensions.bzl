"""Module extension for CC toolchains."""

load("@bzd_platforms//:defs.bzl", "constraints_from_platform_name")
load("//toolchains/clang:defs.bzl", "clang")
load("//toolchains/esp_idf:defs.bzl", "esp32", "esp32s3")
load("//toolchains/gcc:defs.bzl", "gcc")

_repositories = dict(**clang)
_repositories.update(esp32)
_repositories.update(esp32s3)
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
    build_content = """# ---- Toolchain constraint value.
"""

    # Generate the constraint value for the toolchain.
    if repository_ctx.attr.toolchain_constraint:
        build_content += """
alias(
    name = "toolchain",
    actual = "{toolchain_constraint}",
    visibility = ["//visibility:public"],
)
""".format(toolchain_constraint = repository_ctx.attr.toolchain_constraint)
    else:
        build_content += """
constraint_value(
    name = "toolchain",
    constraint_setting = "@bzd_platforms//toolchain",
    visibility = ["//visibility:public"],
)
"""

    build_content += """
platform(
    name = "platform",
    parents = [
        "@platforms//host",
    ],
    constraint_values = [
        ":toolchain",
    ],
    visibility = ["//visibility:public"],
)

# ---- Toolchains (CC & Binary & Test)
"""

    # Add the cc and binary toolchains.
    targets = {}
    configs = _make_configs(repository_ctx.attr.repo_name, repository_ctx.attr.version)
    for repo_name, config in configs.items():
        execution_constraints = constraints_from_platform_name(config.execution)
        target_constraints = constraints_from_platform_name(config.target)
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

toolchain(
    name = "test-{name}",
    exec_compatible_with = [
        {execution_constraints}
    ],
    target_compatible_with = [
        ":toolchain",
        {target_constraints}
    ],
    toolchain = "@bazel_tools//tools/test:empty_toolchain",
    toolchain_type = "@bazel_tools//tools/test:default_test_toolchain_type",
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
        "extra": attr.string(),
        "repo_name": attr.string(mandatory = True),
        "toolchain_constraint": attr.label(),
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
                toolchain_constraint = toolchain.toolchain_constraint,
            )

    fragments = {}

    def use_fragment(fragment, **kwargs):
        if fragment not in fragments:
            result = fragment(**kwargs)
            fragments[fragment] = result
        return fragments[fragment]

    # Build the repositories.
    for name, toolchain in configs.items():
        # Create execution/target specific repositories.
        configs = _make_configs(name, toolchain.version)
        tools = {}
        for repo_name, config in configs.items():
            result = config.factory(module_ctx, repo_name, use_fragment)
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
            toolchain_constraint = toolchain.toolchain_constraint,
            extra = extra,
        )

toolchain_cc = module_extension(
    implementation = _toolchain_cc_impl,
    tag_classes = {
        "toolchain": tag_class(
            attrs = {
                "name": attr.string(mandatory = True),
                "toolchain_constraint": attr.label(doc = "Defines the constraint to select the toolchain."),
                "version": attr.string(values = _repositories.keys(), mandatory = True),
            },
        ),
    },
)
