"""BDL system rules."""

load("@bzd_package//:defs.bzl", "BzdPackageMetadataFragmentInfo")
load("//private:common.bzl", "aspect_bdl_providers", "library_extensions", "make_composition_language_providers", "precompile_bdl", "transition_platform")
load("//private:providers.bzl", "BdlSystemInfo", "BdlSystemJsonInfo", "BdlTargetInfo")

visibility(["//..."])

_BDL_SYSTEM_GENERIC_ATTR = {
    "deps": attr.label_list(
        doc = "Dependencies for the rule.",
    ),
    "platform": None,
    "system": None,
    "target": None,
    "target_name": None,
    "targets": attr.string_keyed_label_dict(
        doc = "A dictionary with name and target corresponding to the binaries for this system.",
        configurable = False,
    ),
    "testonly": attr.bool(
        doc = "Set testonly attribute.",
        configurable = False,
    ),
}

def _bdl_system_impl(ctx):
    # Loop through all the name/target pairs and generate the composition files.
    bdl_providers = {}
    deps = {}
    for name, target in ctx.attr.targets.items():
        target_provider = target[BdlTargetInfo]
        target_name = "{}.{}".format(ctx.label.name, name)

        # Generate the target-specific composition by injecting the new namespace.
        bdl_provider, _ = precompile_bdl(
            ctx = ctx,
            srcs = target_provider.composition,
            deps = target_provider.deps,
            output_dir = target_name,
            namespace = name,
        )
        bdl_providers[name] = bdl_provider
        deps[name] = target_provider.deps

    # Generate the composition files.
    providers = make_composition_language_providers(
        ctx = ctx,
        name = ctx.attr.name,
        deps = ctx.attr.deps,
        target_deps = deps,
        target_bdl_providers = bdl_providers,
    )

    # JSON provider should always be present.
    json = {target: data["files"]["json"] for target, data in providers["json"].items()}

    return [BdlSystemInfo(
        data = providers,
    ), BdlSystemJsonInfo(json = json)]

_bdl_system = rule(
    implementation = _bdl_system_impl,
    doc = """Generate a system from targets.""",
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            allow_files = True,
            doc = "List of dependencies.",
            aspects = [aspect_bdl_providers],
        ),
        "targets": attr.string_keyed_label_dict(
            mandatory = True,
            providers = [BdlTargetInfo],
            doc = "Targets to be included for the system definition.",
        ),
        "_bdl": attr.label(
            default = Label("//bdl"),
            cfg = "exec",
            executable = True,
        ),
    } | {("_deps_" + name): (attr.label_list(
        default = data["composition"]["deps"],
    )) for name, data in library_extensions.items() if "composition" in data},
)

def _bdl_binary_impl(ctx):
    target_provider = ctx.attr.target[BdlTargetInfo]
    system = ctx.attr.system[BdlSystemInfo]
    name = ctx.label.name

    fmt = target_provider.language
    if fmt in library_extensions:
        provider = system.data[fmt][ctx.attr.target_name]
        binary = target_provider.binary
        if binary:
            binary_file = ctx.actions.declare_file(ctx.label.name + ".binary")
            runfiles = ctx.runfiles(
                files = provider.get("files", {}).values() + ctx.files.data,
            ).merge(binary[DefaultInfo].default_runfiles)
            if "runfiles" in provider:
                runfiles = runfiles.merge(provider["runfiles"])
            ctx.actions.write(
                is_executable = True,
                output = binary_file,
                content = "{executable} {args} \"$@\"".format(
                    executable = binary[DefaultInfo].files_to_run.executable.short_path,
                    args = " ".join(["--{}='{}'".format(key, value.short_path) for key, value in provider.get("files", {}).items()]),
                ),
            )
            providers = [
                DefaultInfo(executable = binary_file, runfiles = runfiles),
            ]
            metadata = []
        elif "binary" in library_extensions[fmt]:
            providers, metadata = library_extensions[fmt]["binary"]["build"](ctx, name, provider)
        else:
            fail("No binary associated with target language '{}'.".format(fmt))

    else:
        fail("Unsupported target language '{}'.".format(fmt))

    return [
        BzdPackageMetadataFragmentInfo(
            manifests = metadata,
        ),
        OutputGroupInfo(metadata = metadata),
        ctx.attr.system[BdlSystemJsonInfo],
    ] + providers

def _bzd_binary_generic(is_test):
    return rule(
        implementation = _bdl_binary_impl,
        doc = """Create a binary from a system rule.""",
        attrs = {
            "data": attr.label_list(
                allow_files = True,
                doc = "Files to be added to the runfiles.",
            ),
            "platform": attr.label(
                default = None,
                doc = "The platform used for the transition of this rule.",
            ),
            "system": attr.label(
                mandatory = True,
                doc = "The system rule associated with this target.",
                providers = [BdlSystemInfo, BdlSystemJsonInfo],
            ),
            "target": attr.label(
                mandatory = True,
                doc = "The target label for this binary.",
                providers = [BdlTargetInfo],
            ),
            "target_name": attr.string(
                mandatory = True,
                doc = "The name of the target.",
            ),
            "_allowlist_function_transition": attr.label(
                default = Label("@bazel_tools//tools/allowlists/function_transition_allowlist"),
            ),
            "_debug": attr.label(
                default = Label("@@//tools/bazel_build/settings/debug"),
            ),
            "_executor": attr.label(
                default = Label("@@//tools/bazel_build/settings/executor"),
            ),
            "_map_analyzer_script": attr.label(
                executable = True,
                cfg = "exec",
                default = Label("@bzd_toolchain_cc//binary/map_analyzer"),
            ),
        } | {("_metadata_" + name): (attr.label_list(
            default = data["binary"]["metadata"],
        )) for name, data in library_extensions.items() if "binary" in data},
        toolchains = [
            "@rules_cc//cc:toolchain_type",
            "@bzd_toolchain_cc//binary:toolchain_type",
        ],
        fragments = ["cpp"],
        cfg = transition_platform,
        executable = True,
        test = is_test,
    )

_bdl_binary = _bzd_binary_generic(is_test = False)
_bdl_test = _bzd_binary_generic(is_test = True)

def _bdl_system_entry_point_impl(ctx):
    # Use the first target as the default target for this deployment.
    # It is guaranteed to have at least one target with the mandatory attribute.
    default_target = None
    for _, target in ctx.attr.targets.items():
        default_target = target
        break

    # Create an alias.
    executable = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.symlink(
        output = executable,
        target_file = default_target[DefaultInfo].files_to_run.executable,
    )

    return [
        DefaultInfo(
            executable = executable,
            runfiles = default_target[DefaultInfo].default_runfiles,
            files = depset(transitive = [target[DefaultInfo].files for target in ctx.attr.targets.values()]),
        ),
        default_target[BdlSystemJsonInfo],
    ]

_bdl_system_entry_point = rule(
    implementation = _bdl_system_entry_point_impl,
    doc = """Entry point of a system rule.""",
    attrs = {
        "targets": attr.string_keyed_label_dict(
            mandatory = True,
            doc = "A dictionary with name and executable targets corresponding to the binaries for this system.",
            cfg = "target",
            providers = [BdlSystemJsonInfo],
        ),
    },
    executable = True,
    provides = [BdlSystemJsonInfo, DefaultInfo],
)

def _target_to_platform(target):
    """Convert a target label into its related platform label."""
    if Label(target).name.endswith("auto"):
        return None
    return str(Label(target)) + ".platform"

def _bdl_system_macro_impl(name, targets, testonly, deps, visibility, tags = None, target_compatible_with = None, **kwargs):
    _bdl_system(
        name = "{}.system".format(name),
        targets = targets,
        testonly = testonly,
        deps = deps,
        tags = ["manual"],
    )

    for target_name, target in targets.items():
        _bdl_binary(
            name = "{}.{}".format(name, target_name),
            platform = _target_to_platform(target),
            target = target,
            target_name = target_name,
            testonly = testonly,
            system = "{}.system".format(name),
            visibility = visibility,
            tags = tags,
            target_compatible_with = target_compatible_with,
            **kwargs
        )

    _bdl_system_entry_point(
        name = name,
        targets = {target_name: "{}.{}".format(name, target_name) for target_name, target in targets.items()},
        visibility = visibility,
        tags = tags,
        target_compatible_with = target_compatible_with,
    )

bdl_system = macro(
    implementation = _bdl_system_macro_impl,
    inherit_attrs = _bdl_binary,
    attrs = _BDL_SYSTEM_GENERIC_ATTR,
)

def _bdl_system_test_macro_impl(name, targets, testonly, deps, visibility, **kwargs):
    _bdl_system(
        name = "{}.system".format(name),
        targets = targets,
        testonly = testonly,
        deps = deps,
        tags = ["manual"],
    )

    for target_name, target in targets.items():
        _bdl_test(
            name = "{}.{}".format(name, target_name),
            platform = _target_to_platform(target),
            target = target,
            target_name = target_name,
            testonly = testonly,
            system = "{}.system".format(name),
            visibility = visibility,
            **kwargs
        )

    native.alias(
        name = name,
        actual = "{}.{}".format(name, targets.keys()[0]),
        visibility = visibility,
    )

bdl_system_test = macro(
    implementation = _bdl_system_test_macro_impl,
    inherit_attrs = _bdl_test,
    attrs = _BDL_SYSTEM_GENERIC_ATTR,
)
