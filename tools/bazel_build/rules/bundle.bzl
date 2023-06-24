# Create a bundle with a binary, a self contained, self extractable binary that can run on various hosts.

def _bzd_bundle_impl(ctx):
    binary = ctx.attr.binary
    if binary.files_to_run == None:
        fail("The binary target '{}' must have a FilesToRunProvider".format(binary))
    manifest = binary.files_to_run.runfiles_manifest
    executable = binary.files_to_run.executable
    workspace = binary.label.workspace_name or "_main"

    if binary.default_runfiles == None:
        fail("The binary target '{}' must have a default_runfiles".format(binary))
    inputs = binary.default_runfiles.files

    output = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.run(
        inputs = [manifest] + inputs.to_list(),
        outputs = [output],
        arguments = ["--output", output.path, "--cwd", workspace, manifest.path, executable.short_path],
        executable = ctx.executable._bundle,
    )

    return [
        DefaultInfo(
            files = depset([output]),
        ),
    ]

bzd_bundle = rule(
    implementation = _bzd_bundle_impl,
    attrs = {
        "binary": attr.label(
            mandatory = True,
            cfg = "exec",
            executable = True,
            doc = "The binnary target to bundle.",
        ),
        "_bundle": attr.label(
            default = Label("//tools/bazel_build/rules/assets/bundle"),
            cfg = "exec",
            executable = True,
        ),
    },
)
