load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")

# Custom provider for a manifest
BzdManifestInfo = provider(fields = ["manifest"])

def _bzd_manifest_impl(ctx):
    return [DefaultInfo(), CcInfo(), BzdManifestInfo(manifest = depset(transitive = [f.files for f in ctx.attr.manifest]))]

"""
Rule to declare a bzd manifests.
"""

bzd_manifest = rule(
    implementation = _bzd_manifest_impl,
    attrs = {
        "manifest": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Input manifest files.",
        ),
        "deps": attr.label_list(
            allow_files = True,
        ),
    },
)

def _bzd_deps_aspect_impl(target, ctx):
    if ctx.rule.kind == "bzd_manifest":
        return []
    return [
        BzdManifestInfo(manifest = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.rule.attr.deps])),
    ]

"""
Aspects to gather data from bzd depedencies.
"""
bzd_deps_aspect = aspect(
    implementation = _bzd_deps_aspect_impl,
    attr_aspects = ["deps"],
)

def _bzd_generate_rule_impl(ctx):
    manifests = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.attr.deps]).to_list()

    # Build the argument list
    args = ctx.actions.args()
    args.add("--format")
    args.add(ctx.attr.format)
    args.add("--output")
    args.add(ctx.outputs.output.path)
    outputs = [ctx.outputs.output]

    if ctx.outputs.output_manifest:
        args.add("--manifest")
        args.add(ctx.outputs.output_manifest.path)
        outputs += [ctx.outputs.output_manifest]

    # Add the manifest list
    [args.add(f.path) for f in manifests]

    # Generate the files
    ctx.actions.run(
        outputs = outputs,
        inputs = manifests,
        arguments = [args],
        progress_message = "Generating bzd files...",
        executable = ctx.executable._generator,
    )

    return [
        OutputGroupInfo(all_files = depset(outputs)),
        CcInfo(),
    ]

"""
Rule to define the bzd file generator.
"""
bzd_generate_rule = rule(
    implementation = _bzd_generate_rule_impl,
    output_to_genfiles = True,
    attrs = {
        "deps": attr.label_list(
            aspects = [bzd_deps_aspect],
            doc = "Dependencies used to build this target.",
        ),
        "format": attr.string(
            mandatory = True,
            doc = "Format to be used for the file generation.",
        ),
        "output": attr.output(
            mandatory = True,
            doc = "Define the output file path.",
        ),
        "output_manifest": attr.output(
            doc = "Define the merged manifest output for this file.",
        ),
        "_generator": attr.label(
            default = Label("//tools/bzd/generator"),
            cfg = "host",
            executable = True,
        ),
    },
)

def _bzd_cc_macro_impl(is_test, name, deps, **kwargs):
    # Generates the auto-generated files to be compiled with the project
    output_cc = ".bzd/{}.cpp".format(name)
    output_manifest = ".bzd/{}.manifest".format(name)
    manifest_rule_name = "{}_manifest".format(name)
    bzd_generate_rule(
        name = manifest_rule_name,
        deps = deps,
        format = "cpp",
        output = output_cc,
        output_manifest = output_manifest,
    )

    # Generates a library from the auto-generated files
    library_rule_name = "{}_library".format(name)
    cc_library(
        name = library_rule_name,
        srcs = [output_cc],
        deps = [manifest_rule_name] + deps,
        alwayslink = True,
    )

    # Call the binary/test rule
    rule_to_use = cc_test if is_test else cc_binary
    rule_to_use(
        name = name,
        deps = deps + [library_rule_name],
        **kwargs
    )

def _bzd_pack_impl(ctx):
    binary = ctx.attr.binary
    executable = binary.files_to_run.executable.path

    # Gather toolchain information
    info = ctx.toolchains["//tools/bazel.build/toolchains:toolchain_type"].app

    # Prepare application binary
    prepare_output = ctx.actions.declare_file(".bzd/{}.app".format(ctx.attr.name))
    prepare = info.prepare

    # Run the prepare step
    ctx.actions.run(
        inputs = [binary.files_to_run.executable],
        outputs = [prepare_output],
        tools = prepare.data_runfiles.files,
        use_default_shell_env = True,
        arguments = [executable, prepare_output.path],
        executable = prepare.files_to_run,
    )

    # Generate the info report
    info_report = ctx.actions.declare_file(".bzd/{}.json".format(ctx.attr.name))
    args = [info_report.path]
    ctx.actions.run(
        inputs = [binary.files_to_run.executable, prepare_output],
        outputs = [info_report],
        progress_message = "information report for application {}".format(ctx),
        arguments = args,
        executable = ctx.executable._info_script
    )

    deploy = info.deploy

    ctx.actions.write(
        output = ctx.outputs.executable,
        is_executable = True,
        content = "{} \"{}\" $@".format(deploy.files_to_run.executable.short_path, prepare_output.short_path)
    )

    runfiles = ctx.runfiles(
        files = [prepare_output]
    )
    runfiles = runfiles.merge(binary.default_runfiles)
    runfiles = runfiles.merge(deploy.data_runfiles)

    return DefaultInfo(
        files = depset([info_report]),
        executable = ctx.outputs.executable,
        runfiles = runfiles
    )

bzd_pack = rule(
    implementation = _bzd_pack_impl,
    attrs = {
        "binary": attr.label(
            allow_files = False,
            mandatory = True,
            doc = "Label of the binary to be packed",
        ),
        "_info_script": attr.label(
            executable = True,
            cfg = "host",
            allow_files = True,
            default = Label("//tools/bzd:info_script"),
        )
    },
    executable = True,
    toolchains = ["//tools/bazel.build/toolchains:toolchain_type"]
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
    for interface in ctx.attr.interfaces:
        content += "  \"{}\":\n".format(interface)
        content += "    includes: [{}]\n".format(", ".join(["\"{}\"".format(include) for include in ctx.attr.includes]))

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
        "interfaces": attr.string_list(
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
    genmanifest_name = "genmanifest_{}".format(name)
    output_genmanifest = ".bzd/genmanifest_{}.manifest".format(name)
    _bzd_genmanifest(
        name = genmanifest_name,
        includes = includes,
        interfaces = interfaces,
        output = output_genmanifest,
    )

    # Declare the manifest
    manifest_name = "manifest_{}".format(name)
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
