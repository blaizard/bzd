"""Create a tarball bundle from various sources."""

def _collect_executables(executables):
    all_executables = []
    for prefix, executable in executables.items():
        if executable.files_to_run == None:
            fail("The executable target '{}' (prefix '{}') must have a FilesToRunProvider".format(executable, prefix))
        if executable.default_runfiles == None:
            fail("The executable target '{}' (prefix '{}') must have a default_runfiles".format(executable, prefix))

        all_executables.append(struct(
            manifest = executable.files_to_run.runfiles_manifest,
            entry_point = executable.files_to_run.executable.short_path,
            prefix = prefix,
            files_to_run = executable.files_to_run,
            workspace = executable.label.workspace_name or "_main",
        ))
    return all_executables

def _bzd_bundle_tar_impl(ctx):
    executables = _collect_executables(ctx.attr.executables)
    output = ctx.outputs.output if ctx.attr.output else ctx.actions.declare_file(ctx.label.name)

    args = ctx.actions.args()
    args.add("--output", output)
    for executable in executables:
        args.add_all("--executable", [executable.manifest, executable.prefix, executable.entry_point, executable.workspace])
    if ctx.attr.compression:
        args.add("--compression", ctx.attr.compression)
    if ctx.attr.bootstrap_script:
        args.add("--bootstrap-script", ctx.attr.bootstrap_script)

    ctx.actions.run(
        inputs = [executable.manifest for executable in executables],
        outputs = [output],
        progress_message = "Bundling {}...".format(output.short_path),
        arguments = [args],
        executable = ctx.executable._bundler,
        tools = [executable.files_to_run for executable in executables],
    )

    return [
        DefaultInfo(
            files = depset([output]),
        ),
    ]

bzd_bundle_tar = rule(
    doc = "Bundle a binary and its runfiles into a tar archive.",
    implementation = _bzd_bundle_tar_impl,
    attrs = {
        "bootstrap_script": attr.string(
            doc = "Create a bootstrap script for all executables.",
        ),
        "compression": attr.string(
            values = ["gz", "bz2", "xz"],
            doc = "The type of compression for the resulting tarball.",
        ),
        "executables": attr.string_keyed_label_dict(
            cfg = "target",
            doc = "The executable targets to bundle and their associated prefix.",
        ),
        "output": attr.output(
            doc = "The name of the output tar file, if unset, it will have the same name as the rule.",
        ),
        "_bundler": attr.label(
            default = Label("//private/python:bundler"),
            cfg = "exec",
            executable = True,
        ),
    },
)
