"""Rule to draw a diagram from a bdl architecture."""

load("//:defs.bzl", "BdlSystemJsonInfo")

def _bdl_system_diagram_impl(ctx):
    system_json = ctx.attr.system[BdlSystemJsonInfo].json
    inputs = []
    args = []
    for target, json in system_json.items():
        inputs.append(json)
        args += [target, json.path]

    output = ctx.actions.declare_file("{}.svg".format(ctx.label.name))
    ctx.actions.run(
        inputs = inputs,
        outputs = [output],
        executable = ctx.executable._diagram,
        arguments = ["--output", output.path] + args,
        progress_message = "Generating diagram for {}".format(ctx.label),
    )

    return DefaultInfo(
        files = depset(inputs + [output]),
    )

bdl_system_diagram = rule(
    implementation = _bdl_system_diagram_impl,
    doc = "Extract documentation information from a system.",
    attrs = {
        "system": attr.label(
            mandatory = True,
            doc = "The system rule associated with this target.",
            providers = [BdlSystemJsonInfo],
        ),
        "_diagram": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//diagram"),
        ),
    },
)
