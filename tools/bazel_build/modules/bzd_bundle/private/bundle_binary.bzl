"""Bundle an executable into an self extractable binary."""

load("//private:bundle_tar.bzl", "bzd_bundle_tar")

def _bzd_bundle_binary_script_impl(ctx):
    args = ctx.actions.args()
    args.add("--output", ctx.outputs.executable)
    args.add("--entry-point", ctx.attr.bootstrap_script)
    if ctx.attr.compression:
        args.add("--compression", ctx.attr.compression)
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
        "compression": attr.string(
            values = ["gz", "bz2", "xz"],
            doc = "The type of compression for the resulting tarball.",
        ),
        "_archive_to_script": attr.label(
            default = Label("//private/python:archive_to_script"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)

def _bzd_bundle_binary_impl(name, visibility, compression, compression_level, executable, bootstrap_script, args, **kwargs):
    bzd_bundle_tar(
        name = "{}.bundle".format(name),
        executables = {
            bootstrap_script: executable,
        },
        compression = compression or None,
        compression_level = compression_level,
    )

    _bzd_bundle_binary_script(
        name = name,
        bootstrap_script = bootstrap_script,
        compression = compression or None,
        archive = "{}.bundle".format(name),
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
            configurable = False,
        ),
        "compression": attr.string(
            values = ["gz", "bz2", "xz"],
            doc = "The type of compression for the resulting tarball.",
            configurable = False,
        ),
        "compression_level": attr.int(
            doc = "The compression level for the resulting tarball.",
            default = 6,
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
