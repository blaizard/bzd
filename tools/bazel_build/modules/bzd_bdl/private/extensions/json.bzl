"""JSON extension for bdl rules."""

load("//private/extensions/json:composition.bzl", "generator_json_composition")

def _composition_data(_info, _info_per_target):
    return {}

def _composition_providers(ctx, output, _deps):
    runfiles = ctx.runfiles().merge_all([dep[DefaultInfo].default_runfiles for dep in ctx.attr.deps])
    return {
        # files will be added in the runfiles.
        "files": {
            "json": output,
        },
        # runfiles will be added as runfiles to the binary.
        "runfiles": runfiles,
    }

extension = {
    "json": {
        "composition": {
            "data": _composition_data,
            "deps": [],
            "generator": generator_json_composition,
            "output": "{name}.composition.{target}.json",
            "providers": _composition_providers,
        },
        "display": "Json",
    },
}
