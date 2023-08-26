"""Symlink rule."""

def _bzd_symlink_impl(ctx):
    ctx.actions.symlink(
        output = ctx.outputs.output,
        target_file = ctx.file.input,
    )

    return [DefaultInfo(files = depset([ctx.outputs.output]))]

bzd_symlink = rule(
    implementation = _bzd_symlink_impl,
    attrs = {
        "input": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "Input label or file that should be symlinked.",
        ),
        "output": attr.output(
            mandatory = True,
            doc = "The name of the symlink created by the rule.",
        ),
    },
)
