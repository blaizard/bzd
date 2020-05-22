load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library", "cc_test")
load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel_build/rules:manifest.bzl", "bzd_manifest", "bzd_manifest_build")

def _bzd_cc_macro_impl(is_test, name, deps, **kwargs):
    # Generates the auto-generated files to be compiled with the project
    output_cc = ".bzd/{}.cpp".format(name)
    output_manifest = ".bzd/{}.manifest".format(name)
    manifest_rule_name = "{}_manifest".format(name)
    bzd_manifest_build(
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
    info = ctx.toolchains["//tools/bazel_build/toolchains/cc:toolchain_type"].app

    # --- Prepare phase

    prepare = info.prepare
    if prepare:
        # Run the prepare step only if it is present
        prepare_output = ctx.actions.declare_file(".bzd/{}.app".format(ctx.attr.name))
        ctx.actions.run(
            inputs = [binary.files_to_run.executable],
            outputs = [prepare_output],
            tools = prepare.data_runfiles.files,
            arguments = [executable, prepare_output.path],
            executable = prepare.files_to_run,
        )
    else:
        prepare_output = binary.files_to_run.executable

    # --- Info phase

    info_report = ctx.actions.declare_file(".bzd/{}.json".format(ctx.attr.name))
    args = [info_report.path]
    ctx.actions.run(
        inputs = [binary.files_to_run.executable, prepare_output],
        outputs = [info_report],
        progress_message = "information report for application {}".format(ctx),
        arguments = args,
        executable = ctx.executable._info_script,
    )

    # --- Execution phase

    if info.execute:
        return sh_binary_wrapper_impl(
            ctx = ctx,
            binary = info.execute,
            output = ctx.outputs.executable,
            extra_runfiles = [prepare_output],
            files = depset([info_report]),
            command = "{{binary}} \"{}\" $@".format(prepare_output.short_path),
        )

    # If no executable are set, execute as a normal shell command

    ctx.actions.write(
        output = ctx.outputs.executable,
        is_executable = True,
        content = "exec {} $@".format(prepare_output.short_path),
    )

    return DefaultInfo(
        executable = ctx.outputs.executable,
        runfiles = ctx.runfiles(files = [prepare_output]),
        files = depset([info_report]),
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
            default = Label("//tools/bazel_build/rules/assets/cc:info_script"),
        ),
    },
    executable = True,
    toolchains = ["//tools/bazel_build/toolchains/cc:toolchain_type"],
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
