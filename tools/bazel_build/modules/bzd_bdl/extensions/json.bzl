"""JSON extension for bdl rules."""

def _composition_data(_info, _info_per_target):
    return {}

def _composition_providers(ctx, output, _deps):
    runfiles = ctx.runfiles().merge_all([dep[DefaultInfo].default_runfiles for dep in ctx.attr.deps])
    return {
        # runfiles will be added as runfiles to the binary.
        "runfiles": runfiles,
        # files will be added in the runfiles.
        "files": {
            "json": output,
        },
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
