"""JSON extension for bdl rules."""

def _composition_data(_info, _info_per_target):
    return {}

def _composition_providers(_ctx, output, _deps):
    return {
        "json": output,
    }

extension = {
    "json": {
        "composition": {
            "data": _composition_data,
            "deps": [],
            "output": "{name}.composition.{target}.json",
            "providers": _composition_providers,
        },
        "display": "Json",
    },
}
