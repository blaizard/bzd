"""Embed rule."""

def _bzd_embed_impl(ctx):
    ctx.actions.run(
        inputs = [ctx.file.input],
        outputs = [ctx.outputs.output],
        progress_message = "Generating embed {}...".format(ctx.label),
        arguments = [
            "--output",
            ctx.outputs.output.path,
            "--format",
            ctx.attr.format,
            ctx.file.input.path,
        ],
        executable = ctx.executable._embed,
    )

    return [DefaultInfo(files = depset([ctx.outputs.output]))]

bzd_embed = rule(
    implementation = _bzd_embed_impl,
    attrs = {
        "format": attr.string(
            mandatory = True,
            doc = "The format into which the file should be created.",
        ),
        "input": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "File that should be embedded.",
        ),
        "output": attr.output(
            mandatory = True,
            doc = "The resulting embedable file.",
        ),
        "_embed": attr.label(
            default = Label("@bzd_lib//embed:embed"),
            cfg = "exec",
            executable = True,
        ),
    },
)
