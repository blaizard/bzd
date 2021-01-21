load("@rules_cc//cc:defs.bzl", "cc_test", original_cc_library = "cc_library")
load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel_build/rules:manifest.bzl", "bzd_manifest")
load("//tools/bazel_build/rules:package.bzl", "BzdPackageFragment", "BzdPackageMetadataFragment")
load("@bazel_skylib//lib:new_sets.bzl", "sets")
load("@rules_cc//cc:find_cc_toolchain.bzl", original_find_cc_toolchain = "find_cc_toolchain")
load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")

def find_cc_toolchain(ctx):
    # TODO use find_cc_toolchain, somehow it doesn't work for all configurations.
    # See this related issue: https://github.com/bazelbuild/rules_cc/issues/74
    cc_toolchain = ctx.toolchains["@rules_cc//cc:toolchain_type"]
    if not hasattr(cc_toolchain, "all_files"):
        cc_toolchain = original_find_cc_toolchain(ctx)
    return cc_toolchain

def _cc_run_action(ctx, action, variables = None, inputs = [], args = [], **kwargs):
    """
    Execute an action from the current C++ toolchain.

    Args:
        ctx: Rule context.
        action: C++ action, see ACTION_NAMES.
        variables: Context variables from C++ toolchain.
        inputs: File inputs.
        args: Arguments to be passed to the tool.
    """

    cc_toolchain = find_cc_toolchain(ctx)

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    executable = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = action,
    )

    if variables:
        action_args = cc_common.get_memory_inefficient_command_line(
            feature_configuration = feature_configuration,
            action_name = action,
            variables = variables,
        )
        action_env = cc_common.get_environment_variables(
            feature_configuration = feature_configuration,
            action_name = action,
            variables = variables,
        )
    else:
        action_args = []
        action_env = None

    ctx.actions.run(
        executable = executable,
        env = action_env,
        arguments = action_args + args,
        inputs = depset(
            direct = inputs,
            transitive = [
                cc_toolchain.all_files,
            ],
        ),
        **kwargs
    )

def _cc_linker(ctx, cc_info_providers, map_analyzer):
    """
    Link cc providers and generate metadata from the linker map.

    Args:
        ctx: Rule context.
        cc_info_providers: CcInfo providers.
        map_analyzer: Map analyzer metadata generator.

    Returns:
        A tuple containing the binary file and the metadatas.
    """

    # Build the list of libraries from the cc_info_providers
    library_files = sets.make()
    for li in cc_info_providers.linking_context.linker_inputs.to_list():
        for library_to_link in li.libraries:
            for library in [library_to_link.static_library, library_to_link.pic_static_library]:
                if library:
                    sets.insert(library_files, library)
    library_files = sets.to_list(library_files)

    # Get the C++ toolchain and the feature configuration.
    cc_toolchain = find_cc_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    # Output binary file.
    binary_file = ctx.actions.declare_file("{}.original".format(ctx.label.name))
    link_variables = cc_common.create_link_variables(
        cc_toolchain = cc_toolchain,
        feature_configuration = feature_configuration,
        output_file = binary_file.path,
    )

    # Run the linker stage.
    map_file = ctx.actions.declare_file("{}.map".format(ctx.attr.name))
    _cc_run_action(
        ctx = ctx,
        action = ACTION_NAMES.cpp_link_executable,
        variables = link_variables,
        inputs = library_files,
        args = ["-Wl,-Map={}".format(map_file.path)] + [f.path for f in library_files],
        outputs = [binary_file, map_file],
    )

    # Analyze the map file.
    metadata_file = ctx.actions.declare_file("{}.map.metadata".format(ctx.attr.name))
    ctx.actions.run(
        inputs = [map_file],
        outputs = [metadata_file],
        progress_message = "Generating metadata for {}".format(ctx.attr.name),
        arguments = [map_file.path, metadata_file.path],
        tools = map_analyzer.data_runfiles.files,
        executable = map_analyzer.files_to_run,
    )

    return binary_file, [metadata_file]

def _cc_binary(ctx, binary_file):
    """
    Prepare the binary for the execution stage.

    Returns:
        A tuple containing the DefaultInfo provider and the metadatas.
    """

    binary_toolchain = ctx.toolchains["//tools/bazel_build/toolchains/binary:toolchain_type"].info

    # Application binary prepare stage

    prepare = binary_toolchain.prepare
    if prepare:
        # Run the prepare step only if it is present
        final_binary_file = ctx.actions.declare_file("{}.binary.final".format(ctx.attr.name))
        ctx.actions.run(
            inputs = [binary_file],
            outputs = [final_binary_file],
            tools = prepare.data_runfiles.files,
            arguments = [binary_file.path, final_binary_file.path],
            executable = prepare.files_to_run,
        )
    else:
        final_binary_file = binary_file

    # Application metadata phase

    #metadata_list = binary_toolchain.metadatas if binary_toolchain.metadatas else [ctx.attr._metadata_script]

    """
    for index, metadata in enumerate(metadata_list):
        is_executable = bool(OutputGroupInfo in metadata)
        if is_executable:
            metadata_file = ctx.actions.declare_file(".bzd/{}.metadata.{}".format(ctx.attr.name, index))
            ctx.actions.run(
                inputs = [executable, binary_final],
                outputs = [metadata_file],
                progress_message = "Generating metadata for {}".format(ctx.attr.name),
                arguments = [executable.path, binary_stripped.path, binary_final.path, metadata_file.path],
                tools = metadata.data_runfiles.files,
                executable = metadata.files_to_run,
            )
            metadata_manifests.append(metadata_file)
        else:
            metadata_manifests.append(metadata.files.to_list()[0])
    """

    # Application execution phase

    if binary_toolchain.execute:
        default_info = sh_binary_wrapper_impl(
            ctx = ctx,
            binary = binary_toolchain.execute,
            output = ctx.outputs.executable,
            extra_runfiles = [final_binary_file],
            command = "{{binary}} \"{}\" $@".format(final_binary_file.short_path),
        )

        # If no executable are set, execute as a normal shell command
    else:
        ctx.actions.write(
            output = ctx.outputs.executable,
            is_executable = True,
            content = "exec {} $@".format(final_binary_file.short_path),
        )
        default_info = DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = ctx.runfiles(files = [final_binary_file]),
        )

    return default_info, []

def _bzd_cc_generic_impl(ctx):
    # Gather all CC providers from all dependencies
    cc_info_providers = sets.make()
    for dep in ctx.attr.deps:
        sets.insert(cc_info_providers, dep[CcInfo])
    cc_info_providers = cc_common.merge_cc_infos(cc_infos = sets.to_list(cc_info_providers))

    # Link the CcInfo providers and generate metadata.
    binary_file, link_metadata_files = _cc_linker(ctx, cc_info_providers, ctx.attr._map_analyzer_script)

    # Strip the binary.
    stripped_binary_file = ctx.actions.declare_file("{}.stripped".format(ctx.label.name))
    _cc_run_action(
        ctx = ctx,
        action = ACTION_NAMES.strip,
        inputs = [binary_file],
        args = ["-o", stripped_binary_file.path, binary_file.path],
        outputs = [stripped_binary_file],
    )

    # Make the executor from the binary toolchain
    default_info, binary_metadata_files = _cc_binary(ctx, stripped_binary_file)

    return [
        default_info,
        BzdPackageFragment(
            files = [binary_file],
        ),
        BzdPackageMetadataFragment(
            manifests = ctx.files._default_metadata_files + link_metadata_files + binary_metadata_files,
        ),
    ]

def _bzd_cc_generic(is_test):
    return rule(
        implementation = _bzd_cc_generic_impl,
        attrs = {
            "deps": attr.label_list(
                doc = "C++ dependencies",
                providers = [CcInfo],
                default = [],
            ),
            "_map_analyzer_script": attr.label(
                executable = True,
                cfg = "host",
                default = Label("//tools/bazel_build/rules/assets/cc/map_analyzer"),
            ),
            "_default_metadata_files": attr.label_list(
                default = [
                    Label("//tools/bazel_build/rules/assets/cc:metadata_json"),
                ],
                allow_files = True,
            ),
            "_cc_toolchain": attr.label(default = Label("@rules_cc//cc:current_cc_toolchain")),
        },
        executable = True,
        test = is_test,
        toolchains = [
            "@rules_cc//cc:toolchain_type",
            "//tools/bazel_build/toolchains/binary:toolchain_type",
        ],
        fragments = ["cpp"],
    )

_bzd_cc_binary = _bzd_cc_generic(is_test = False)
_bzd_cc_test = _bzd_cc_generic(is_test = True)

"""
Rule to define a bzd C++ library.
"""

def cc_library(deps = [], **kwargs):
    original_cc_library(
        deps = deps + ["//cc:includes"],
        **kwargs
    )

"""
Rule to define a bzd C++ binary.
"""

def bzd_cc_binary(name, tags = [], **kwags):
    cc_library(
        name = name + ".library",
        tags = tags + ["cc"],
        **kwags
    )
    _bzd_cc_binary(
        name = name,
        tags = tags + ["cc"],
        deps = [name + ".library"],
    )

"""
Rule to define a bzd C++ test.
"""

def bzd_cc_test(name, tags = [], **kwags):
    cc_test(
        name = name,
        tags = tags + ["cc"],
        **kwags
    )

def _bzd_genmanifest_impl(ctx):
    # Create the manifest content
    content = "interfaces:\n"
    for interface, implementationName in ctx.attr.interfaces.items():
        content += "  \"{}\":\n".format(interface)
        content += "    includes: [{}]\n".format(", ".join(["\"{}\"".format(include) for include in ctx.attr.includes]))
        if implementationName != "*":
            content += "    implementation: !ref {}\n".format(implementationName)

    # Create the file
    ctx.actions.write(
        output = ctx.outputs.output,
        content = content,
    )

    return [DefaultInfo(files = depset([ctx.outputs.output]))]

"""
Rule to generate a manifest
"""
_bzd_genmanifest = rule(
    implementation = _bzd_genmanifest_impl,
    attrs = {
        "interfaces": attr.string_dict(
            mandatory = True,
            doc = "List of interfaces availaable through this library.",
        ),
        "includes": attr.string_list(
            doc = "List of includes associated with this library.",
        ),
        "output": attr.output(
            mandatory = True,
            doc = "Define the output manifest path.",
        ),
    },
)

def _bzd_cc_library_impl(name, interfaces, includes, deps, **kwargs):
    # Create the manifest file
    genmanifest_name = "{}.manifest.auto".format(name)
    output_genmanifest = ".bzd/{}.manifest.auto".format(name)
    _bzd_genmanifest(
        name = genmanifest_name,
        includes = includes,
        interfaces = interfaces,
        output = output_genmanifest,
    )

    # Declare the manifest
    manifest_name = "{}.manifest".format(name)
    bzd_manifest(
        name = manifest_name,
        manifest = [output_genmanifest],
    )

    # Associate the manifest with the library
    cc_library(
        name = name,
        deps = deps + [manifest_name],
        **kwargs
    )

"""
Rule to define a bzd library.
"""

def bzd_cc_library(name, interfaces, includes, deps, **kwargs):
    return _bzd_cc_library_impl(name, interfaces, includes, deps, **kwargs)
