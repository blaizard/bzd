"""Manage secrets as a file."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_secret_impl(ctx):
    file = ctx.actions.declare_file("{}.check".format(ctx.label.name))
    ctx.actions.run(
        inputs = [ctx.file.src],
        outputs = [file],
        arguments = [
            "--file",
            ctx.file.src.path,
            "--output",
            file.path,
            "check",
        ],
        progress_message = "Checking secret for {}...".format(ctx.label),
        executable = ctx.executable._secret,
    )
    return [DefaultInfo(files = depset([file]))]

_bzd_secret = rule(
    doc = "Check that the given file is encrypted.",
    implementation = _bzd_secret_impl,
    attrs = {
        "src": attr.label(
            doc = "The secret to be held.",
            allow_single_file = True,
            mandatory = True,
        ),
        "_secret": attr.label(
            doc = "The secret CLI.",
            executable = True,
            cfg = "exec",
            default = Label("//private/secret"),
        ),
    },
)

def _bzd_secret_decrypt_impl(ctx):
    return sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr._secret: "secret",
            ctx.file.src: "file",
        },
        data = [ctx.file.src],
        output = ctx.outputs.executable,
        command = "{secret} --file {file} decrypt",
    )

_bzd_secret_decrypt = rule(
    doc = "Decrypt a secret.",
    implementation = _bzd_secret_decrypt_impl,
    attrs = {
        "src": attr.label(
            doc = "The secret to be seen.",
            allow_single_file = True,
            mandatory = True,
        ),
        "_secret": attr.label(
            doc = "The secret CLI.",
            executable = True,
            cfg = "exec",
            default = Label("//private/secret"),
        ),
    },
    executable = True,
)

def _bzd_secret_macro_impl(name, visibility, src, **kwargs):
    _bzd_secret(
        name = name,
        src = src,
        visibility = visibility,
        **kwargs
    )
    _bzd_secret_decrypt(
        name = "{}.decrypt".format(name),
        src = src,
    )

bzd_secret = macro(
    doc = "Hold a secret and ensure it is encrypted. Additionaly, provide a target to decrypt the secret.",
    implementation = _bzd_secret_macro_impl,
    inherit_attrs = _bzd_secret,
)
