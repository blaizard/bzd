"""JSON extension for bdl rules."""

def _binary_build(ctx, name, provider):
    binary = ctx.actions.declare_file(ctx.label.name + "." + name + ".binary")
    ctx.actions.run_shell(
        inputs = [provider["json"]],
        outputs = [binary],
        command = "",
    )
    return binary, [], []

def _composition_data(_info, _info_per_target):
    return {}

def _composition_providers(_ctx, output, _deps):
    return {
        "json": output,
    }

extension = {
    "json": {
        "binary": {
            "build": _binary_build,
            "metadata": [
            ],
        },
        "composition": {
            "data": _composition_data,
            "deps": [],
            "output": "{name}.composition.{target}.json",
            "providers": _composition_providers,
        },
        "display": "Json",
    },
}
