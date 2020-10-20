load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel_build/rules:manifest.bzl", "bzd_manifest", "bzd_manifest_build")
load("//tools/bazel_build/rules:package.bzl", "BzdPackageFragment", "BzdPackageMetadataFragment")

def _bzd_cc_pack_impl(ctx):
    binary = ctx.attr.dep
    executable = binary.files_to_run.executable

    # Gather toolchain application information
    application = ctx.toolchains["//tools/bazel_build/toolchains/cc:toolchain_type"].app

    # --- Strip the binary

    executable_stripped = ctx.file.dep_stripped

    # --- Prepare phase

    prepare = application.prepare
    if prepare:
        # Run the prepare step only if it is present
        executable_final = ctx.actions.declare_file("{}.binary.final".format(ctx.attr.name))
        ctx.actions.run(
            inputs = [executable_stripped],
            outputs = [executable_final],
            tools = prepare.data_runfiles.files,
            arguments = [executable_stripped.path, executable_final.path],
            executable = prepare.files_to_run,
        )
    else:
        executable_final = executable_stripped

    # --- Metadata phase

    metadata_list = application.metadatas if application.metadatas else [ctx.attr._metadata_script]
    metadata_manifests = [ctx.file._metadata_json]

    for index, metadata in enumerate(metadata_list):
        is_executable = bool(OutputGroupInfo in metadata)
        if is_executable:
            metadata_file = ctx.actions.declare_file(".bzd/{}.metadata.{}".format(ctx.attr.name, index))
            ctx.actions.run(
                inputs = [executable, executable_final],
                outputs = [metadata_file],
                progress_message = "Generating metadata for {}".format(ctx.attr.name),
                arguments = [executable.path, executable_stripped.path, executable_final.path, metadata_file.path],
                tools = metadata.data_runfiles.files,
                executable = metadata.files_to_run,
            )
            metadata_manifests.append(metadata_file)
        else:
            metadata_manifests.append(metadata.files.to_list()[0])

    # --- Execution phase

    if application.execute:
        default_info = sh_binary_wrapper_impl(
            ctx = ctx,
            binary = application.execute,
            output = ctx.outputs.executable,
            extra_runfiles = [executable_final],
            command = "{{binary}} \"{}\" $@".format(executable_final.short_path),
        )

        # If no executable are set, execute as a normal shell command
    else:
        ctx.actions.write(
            output = ctx.outputs.executable,
            is_executable = True,
            content = "exec {} $@".format(executable_final.short_path),
        )
        default_info = DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = ctx.runfiles(files = [executable_final]),
        )

    return [
        default_info,
        BzdPackageFragment(
            files = [executable_final],
        ),
        BzdPackageMetadataFragment(
            manifests = metadata_manifests,
        ),
    ]

_bzd_cc_pack = rule(
    implementation = _bzd_cc_pack_impl,
    attrs = {
        "dep": attr.label(
            allow_files = False,
            mandatory = True,
            doc = "Label of the binary to be packed",
        ),
        "dep_stripped": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "Label of the stripped binary to be packed",
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
        dep = name + ".binary",
        dep_stripped = name + ".binary.stripped",
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
