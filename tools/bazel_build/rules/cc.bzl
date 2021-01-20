load("@rules_cc//cc:defs.bzl", original_cc_library = "cc_library")
load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel_build/rules:manifest.bzl", "bzd_manifest", "bzd_manifest_build")
load("//tools/bazel_build/rules:package.bzl", "BzdPackageFragment", "BzdPackageMetadataFragment")
load("@bazel_skylib//lib:new_sets.bzl", "sets")
load("@rules_cc//cc:find_cc_toolchain.bzl", "find_cc_toolchain")
load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")

def _collect_libs(cc_linking):
    libs = sets.make()
    for li in cc_linking.linker_inputs.to_list():
        for library_to_link in li.libraries:
            for library in [library_to_link.static_library, library_to_link.pic_static_library]:
                if library:
                    sets.insert(libs, library)
    return sets.to_list(libs)

def _cc_link_impl(ctx):
    # Gather all CC providers from all dependencies
    cc_info_providers = sets.make()
    for dep in ctx.attr.deps:
        sets.insert(cc_info_providers, dep[CcInfo])
    cc_info_providers = cc_common.merge_cc_infos(cc_infos = sets.to_list(cc_info_providers))

    # Build the list of library files
    libs = _collect_libs(cc_info_providers.linking_context)

    # TODO use find_cc_toolchain, somehow it doesn't work for all configurations.
    # See this related issue: https://github.com/bazelbuild/rules_cc/issues/74
    cc_toolchain = ctx.toolchains["@rules_cc//cc:toolchain_type"]
    if not hasattr(cc_toolchain, "all_files"):
        cc_toolchain = find_cc_toolchain(ctx)

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    binary_file = ctx.actions.declare_file(ctx.label.name)
    link_variables = cc_common.create_link_variables(
        cc_toolchain = cc_toolchain,
        feature_configuration = feature_configuration,
        output_file = binary_file.path,
    )

    args = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_link_executable,
        variables = link_variables,
    )

    linker_path = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_link_executable,
    )

    env = cc_common.get_environment_variables(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_link_executable,
        variables = link_variables,
    )

    # Run the linker stage.
    map_file = ctx.actions.declare_file("{}.map".format(ctx.attr.name))
    ctx.actions.run(
        executable = linker_path,
        arguments = args + ["-Wl,-Map={}".format(map_file.path)] + [f.path for f in libs],
        env = env,
        inputs = depset(
            direct = libs,
            transitive = [
                cc_toolchain.all_files,
            ],
        ),
        outputs = [binary_file, map_file],
    )

    # Analyze the map file.
    metadata_file = ctx.actions.declare_file(".bzd/{}.metadata.map".format(ctx.attr.name))
    script = ctx.attr._map_analyzer_script
    ctx.actions.run(
        inputs = [map_file],
        outputs = [metadata_file],
        progress_message = "Generating metadata for {}".format(ctx.attr.name),
        arguments = [map_file.path, metadata_file.path],
        tools = script.data_runfiles.files,
        executable = script.files_to_run,
    )

    return [
        DefaultInfo(executable = binary_file),
        BzdPackageFragment(
            files = [binary_file],
        ),
        BzdPackageMetadataFragment(
            manifests = [metadata_file],
        ),
    ]

def _cc_link(is_test):
    return rule(
        implementation = _cc_link_impl,
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
            "_cc_toolchain": attr.label(default = Label("@rules_cc//cc:current_cc_toolchain")),
        },
        executable = True,
        test = is_test,
        toolchains = [
            "@rules_cc//cc:toolchain_type",
        ],
        fragments = ["cpp"],
    )

cc_link_binary = _cc_link(is_test = False)
cc_link_test = _cc_link(is_test = True)

def cc_library(deps = [], **kwargs):
    original_cc_library(
        deps = deps + ["//cc:includes"],
        **kwargs
    )

def cc_binary(name, tags = [], **kwags):
    cc_library(
        name = name + ".library",
        tags = tags + ["cc"],
        **kwags
    )
    cc_link_binary(
        name = name,
        tags = tags + ["cc"],
        deps = [name + ".library"],
    )

def cc_test(name, tags = [], **kwags):
    cc_library(
        name = name + ".library",
        tags = tags + ["cc"],
        **kwags
    )
    cc_link_test(
        name = name,
        tags = tags + ["cc"],
        deps = [name + ".library"],
    )

def _bzd_cc_pack_impl(ctx):
    binary = ctx.attr.binary
    executable = binary.files_to_run.executable

    # Gather toolchain information
    app_toolchain = ctx.toolchains["//tools/bazel_build/toolchains/cc:toolchain_type"].app

    # --- Strip the binary

    binary_stripped = executable  #ctx.file.binary_stripped

    # --- Prepare phase

    prepare = app_toolchain.prepare
    if prepare:
        # Run the prepare step only if it is present
        binary_final = ctx.actions.declare_file("{}.binary.final".format(ctx.attr.name))
        ctx.actions.run(
            inputs = [binary_stripped],
            outputs = [binary_final],
            tools = prepare.data_runfiles.files,
            arguments = [binary_stripped.path, binary_final.path],
            executable = prepare.files_to_run,
        )
    else:
        binary_final = binary_stripped

    # --- Metadata phase

    metadata_list = app_toolchain.metadatas if app_toolchain.metadatas else [ctx.attr._metadata_script]
    metadata_manifests = [ctx.file._metadata_json]

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

    # --- Execution phase

    if app_toolchain.execute:
        default_info = sh_binary_wrapper_impl(
            ctx = ctx,
            binary = app_toolchain.execute,
            output = ctx.outputs.executable,
            extra_runfiles = [binary_final],
            command = "{{binary}} \"{}\" $@".format(binary_final.short_path),
        )

        # If no executable are set, execute as a normal shell command
    else:
        ctx.actions.write(
            output = ctx.outputs.executable,
            is_executable = True,
            content = "exec {} $@".format(binary_final.short_path),
        )
        default_info = DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = ctx.runfiles(files = [binary_final]),
        )

    return [
        default_info,
        BzdPackageFragment(
            files = [binary_final],
        ),
        BzdPackageMetadataFragment(
            manifests = metadata_manifests,
        ),
    ]

_bzd_cc_pack = rule(
    implementation = _bzd_cc_pack_impl,
    attrs = {
        "binary": attr.label(
            allow_files = False,
            mandatory = True,
            doc = "Label of the binary to be packed",
        ),
        "_metadata_script": attr.label(
            executable = True,
            cfg = "host",
            allow_files = True,
            default = Label("//tools/bazel_build/rules/assets/cc:profiler"),
        ),
        "_metadata_json": attr.label(
            default = Label("//tools/bazel_build/rules/assets/cc:metadata_json"),
            allow_single_file = True,
        ),
    },
    executable = True,
    toolchains = [
        "//tools/bazel_build/toolchains/cc:toolchain_type",
    ],
)

def _bzd_cc_macro_impl(is_test, name, deps, **kwargs):
    # Generates the auto-generated files to be compiled with the project
    output_cc = ".bzd/{}.cc".format(name)
    output_manifest = ".bzd/{}.yml".format(name)
    bzd_manifest_build(
        name = name + ".manifest",
        deps = deps,
        format = "cc",
        output = output_cc,
        output_manifest = output_manifest,
    )

    # Generates a library from the auto-generated files
    cc_library(
        name = name + ".library",
        srcs = [output_cc],
        deps = [name + ".manifest"] + deps,
        alwayslink = True,
    )

    # Call the binary/test rule
    rule_to_use = cc_test if is_test else cc_binary
    rule_to_use(
        name = name + ".binary",
        deps = deps + [name + ".library"],
        **kwargs
    )

    _bzd_cc_pack(
        name = name,
        binary = name + ".binary",
    )

"""
Rule to define a bzd binary.
"""

def bzd_cc_binary(name, deps, **kwargs):
    return _bzd_cc_macro_impl(False, name, deps, **kwargs)

"""
Rule to define a bzd test.
"""

def bzd_cc_test(name, deps, **kwargs):
    return _bzd_cc_macro_impl(True, name, deps, **kwargs)

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
