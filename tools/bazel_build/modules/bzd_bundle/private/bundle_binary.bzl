"""Bundle an executable into an self extractable binary."""

load("//private:bundle_tar.bzl", "bzd_bundle_tar")

def _bzd_bundle_binary_script_impl(ctx):
    args = ctx.actions.args()
    args.add("--output", ctx.outputs.executable)
    args.add("--entry-point", ctx.attr.bootstrap_script)
    if ctx.attr.compression:
        args.add("--compression", "gz")
    args.add(ctx.file.archive)

    ctx.actions.run(
        inputs = [ctx.file.archive],
        outputs = [ctx.outputs.executable],
        progress_message = "Bundling self extractable binary {}...".format(str(ctx.label)),
        arguments = [
            args,
            "--",
        ] + ctx.attr.arguments,
        executable = ctx.executable._archive_to_script,
    )

    return [DefaultInfo(
        executable = ctx.outputs.executable,
        files = depset([ctx.outputs.executable]),
    )]

_bzd_bundle_binary_script = rule(
    doc = "Bundle an archive into an executable script.",
    implementation = _bzd_bundle_binary_script_impl,
    attrs = {
        "archive": attr.label(
            doc = "The archive to be bundled.",
            allow_single_file = True,
        ),
        "arguments": attr.string_list(
            doc = "List of arguments to be embedded in the bootstrap script.",
        ),
        "bootstrap_script": attr.string(
            doc = "Entry point of the executable.",
        ),
        "compression": attr.bool(
            doc = "If the tarball is compressed or not.",
        ),
        "_archive_to_script": attr.label(
            default = Label("//private/python:archive_to_script"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)

def _bzd_bundle_binary_impl(name, visibility, compression, executable, bootstrap_script, args, **kwargs):
    archive_name = "{}.tar.gz".format(name) if compression else "{}.tar".format(name)
    bzd_bundle_tar(
        name = "{}.bundle".format(name),
        executables = {
            "": executable,
        },
        bootstrap_script = bootstrap_script,
        compression = "gz" if compression else None,
        output = archive_name,
    )

    _bzd_bundle_binary_script(
        name = name,
        bootstrap_script = bootstrap_script,
        compression = compression,
        archive = archive_name,
        visibility = visibility,
        arguments = args,
        **kwargs
    )

bzd_bundle_binary = macro(
    doc = "Bundle a binary and its runfiles into self contained file.",
    implementation = _bzd_bundle_binary_impl,
    inherit_attrs = _bzd_bundle_binary_script,
    attrs = {
        "archive": None,
        "args": attr.string_list(
            doc = "List of arguments to be embedded in the bootstrap script.",
        ),
        "arguments": None,
        "bootstrap_script": attr.string(
            doc = "Entry point of the executable.",
            default = ".bzd_bundle_binary_bootstrap",
        ),
        "compression": attr.bool(
            doc = "If the tarball is compressed or not.",
            default = True,
            configurable = False,
        ),
        "executable": attr.label(
            cfg = "target",
            executable = True,
            doc = "The executable target to bundle.",
            configurable = False,
        ),
    },
)
