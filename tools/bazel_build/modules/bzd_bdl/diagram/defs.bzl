"""Rule to draw a diagram from a bdl architecture."""

load("@bzd_rules_doc//doc:defs.bzl", "DocumentationExtensionsInfo", "DocumentationInfo")
load("//:defs.bzl", "BdlSystemJsonInfo")

def _bdl_system_diagram_impl(ctx):
    system_json = ctx.attr.system[BdlSystemJsonInfo].json

    extension = ctx.actions.declare_file("{}.extension.json".format(ctx.label.name))
    ctx.actions.write(
        output = extension,
        content = json.encode({
            "//{}:{}".format(ctx.label.package, ctx.label.name): {
                "args": [bdl.path for bdl in system_json.values()],
                "executable": ctx.executable._diagram_markdown.path,
            },
        }),
    )

    return [ctx.attr._diagram_library[DocumentationInfo], DocumentationExtensionsInfo(
        json = depset([extension]),
        tools = depset([ctx.attr._diagram_markdown[DefaultInfo].files_to_run]),
        data = depset(system_json.values()),
    )]

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
        "_diagram_library": attr.label(
            default = Label("//diagram:doc_library"),
        ),
        "_diagram_markdown": attr.label(
            executable = True,
            cfg = "exec",
            default = Label("//diagram:markdown"),
        ),
    },
)
