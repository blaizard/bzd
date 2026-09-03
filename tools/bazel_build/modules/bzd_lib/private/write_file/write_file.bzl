"""Write a file with the given content and substitutions."""

def _expand_substitutions(ctx):
    """Expand the substitutions and return them as a dictionary."""

    substitutions = {}
    for key, value in ctx.attr.substitutions.items():
        value = ctx.expand_location(value, targets = ctx.attr.data)
        substitutions[key] = value
    return substitutions

def _bzd_write_file_impl(ctx):
    """Write a file with the given content and substitutions."""

    if bool(ctx.attr.src) == bool(ctx.attr.content):
        fail("Exactly one of 'src' or 'content' must be specified.")

    substitutions = _expand_substitutions(ctx)
    output = ctx.outputs.output if ctx.attr.output else ctx.actions.declare_file(ctx.label.name)

    if ctx.attr.src:
        template = ctx.file.src
    else:
        template = ctx.actions.declare_file(ctx.label.name + ".content")
        ctx.actions.write(
            output = template,
            content = ctx.attr.content,
        )

    ctx.actions.expand_template(
        output = output,
        template = template,
        substitutions = substitutions,
    )

    return [DefaultInfo(files = depset([output]))]

bzd_write_file = rule(
    doc = "Write a file with the given content and substitutions.",
    implementation = _bzd_write_file_impl,
    attrs = {
        "content": attr.string(
            doc = "The content of the file to be written.",
        ),
        "data": attr.label_list(
            doc = "The data files to be used for location expansion.",
            allow_files = True,
        ),
        "output": attr.output(
            doc = "The output file to be written.",
        ),
        "src": attr.label(
            doc = "The source file to be written.",
            allow_single_file = True,
        ),
        "substitutions": attr.string_dict(
            doc = "The substitutions to be applied to the content.",
        ),
    },
)
