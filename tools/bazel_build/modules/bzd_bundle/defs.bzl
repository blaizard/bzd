"""Create a bundle with a binary, a self contained, self extractable binary that can run on various hosts."""

def _bzd_bundle_impl(ctx):
    binary = ctx.attr.binary
    if binary.files_to_run == None:
        fail("The binary target '{}' must have a FilesToRunProvider".format(binary))
    manifest = binary.files_to_run.runfiles_manifest
    executable = binary.files_to_run.executable
    workspace = binary.label.workspace_name or "_main"

    if binary.default_runfiles == None:
        fail("The binary target '{}' must have a default_runfiles".format(binary))

    ctx.actions.run(
        inputs = [manifest],
        outputs = [ctx.outputs.executable],
        progress_message = "Bundling {}...".format(str(binary.label)),
        arguments = ["--output", ctx.outputs.executable.path, "--cwd", workspace, manifest.path, executable.short_path],
        executable = ctx.executable._bundler,
        tools = [binary.files_to_run],
    )

    return [
        DefaultInfo(
            executable = ctx.outputs.executable,
            files = depset([ctx.outputs.executable]),
        ),
    ]

bzd_bundle = rule(
    implementation = _bzd_bundle_impl,
    attrs = {
        "binary": attr.label(
            mandatory = True,
            cfg = "target",
            executable = True,
            doc = "The binnary target to bundle.",
        ),
        "_bundler": attr.label(
            default = Label("//:bundler"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)
