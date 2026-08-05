"""Bdl composition to JSON."""

load("@bzd_bdl//:providers.bzl", "BdlCompositionInfo", "BdlTargetInfo")

def _generator_json_composition_impl(ctx):
    info = ctx.attr.composition[BdlCompositionInfo]

    output = ctx.actions.declare_file("{}.json".format(ctx.label.name))
    args = ctx.actions.args()
    args.add("--output", output)
    args.add("--target", ctx.attr.target_name)

    sources = [source[1] for provider in info.bdls for source in provider.sources.to_list()]
    args.add_all(sources)

    ctx.actions.run(
        inputs = sources,
        outputs = [output],
        progress_message = "Composing JSON from BDL for {} in {}".format(ctx.attr.target_name, ctx.label),
        arguments = [args],
        executable = ctx.executable._tool,
    )

    return [DefaultInfo(files = depset([output]))]

generator_json_composition = rule(
    implementation = _generator_json_composition_impl,
    doc = """Generate JSON composition files for each targets.""",
    attrs = {
        "composition": attr.label(
            mandatory = True,
            providers = [BdlCompositionInfo],
            doc = "The system composition.",
        ),
        "deps": attr.label_list(
            doc = "List of bdl dependencies.",
        ),
        "target": attr.label(
            mandatory = True,
            providers = [BdlTargetInfo],
            doc = "The target to focus on.",
        ),
        "target_name": attr.string(
            mandatory = True,
            doc = "The name of the target in the system.",
        ),
        "_tool": attr.label(
            default = Label("//private/extensions/json:composition"),
            cfg = "exec",
            executable = True,
        ),
    },
)
