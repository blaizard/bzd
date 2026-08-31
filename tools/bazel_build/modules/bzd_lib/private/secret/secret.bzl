"""Manage secrets as a file."""

load("//config:defs.bzl", "ConfigSourceInfo")

def _bzd_secret_impl(ctx):
    inputs = []
    args = None
    if ctx.attr.src:
        inputs = [ctx.file.src]
        args = ["--file", ctx.file.src.path]
    if ctx.attr.content:
        if args:
            fail("'src' or 'content' must be set, not both.")
        args = ["--payload", "hello", ctx.attr.content]

    if not args:
        fail("'src' or 'content' must be set.")

    file = ctx.actions.declare_file("{}.check".format(ctx.label.name))
    ctx.actions.run(
        inputs = inputs,
        outputs = [file],
        arguments = args + [
            "--output",
            file.path,
            "check",
        ],
        progress_message = "Checking secret for {}...".format(ctx.label),
        executable = ctx.executable._secret,
    )
    return [
        DefaultInfo(files = depset([file])),
        ConfigSourceInfo(
            file = ctx.file.src,
            content = ctx.attr.content,
            metadata = ["secret"],
        ),
    ]

bzd_secret = rule(
    doc = "Check that the given file is encrypted.",
    implementation = _bzd_secret_impl,
    attrs = {
        "content": attr.string(
            doc = "The content of the secret.",
        ),
        "src": attr.label(
            doc = "The secret to be held.",
            allow_single_file = True,
        ),
        "_secret": attr.label(
            doc = "The secret CLI.",
            executable = True,
            cfg = "exec",
            default = Label("//private/secret"),
        ),
    },
)
