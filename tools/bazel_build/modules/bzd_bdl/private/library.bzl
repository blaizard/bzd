"""BDL library rules."""

load("@bdl_extension//:extensions.bzl", "extensions")
load("//private:common.bzl", "aspect_bdl_providers", "precompile_bdl")
load("//private:providers.bzl", "BdlInfo")

visibility(["//..."])

def _bdl_precompile_library_impl(ctx):
    # Pre-compile the BDLs into their language agnostics format.
    bdl_provider, _metadata = precompile_bdl(
        ctx = ctx,
        srcs = ctx.files.srcs,
        deps = ctx.attr.deps,
        presets = ctx.files.presets,
    )
    return [bdl_provider]

_bdl_precompile_library = rule(
    implementation = _bdl_precompile_library_impl,
    doc = """Bzd Description Language generator rule.""",
    attrs = {
        "deps": attr.label_list(
            providers = [BdlInfo],
            aspects = [aspect_bdl_providers],
            doc = "List of bdl dependencies.",
        ),
        "presets": attr.label_list(
            allow_files = [".json"],
            doc = "JSON preset files referenced by `preset NAME from \"PATH\";` statements.",
        ),
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of Bzd Description Language (bdl) files to be included.",
        ),
        "_bdl": attr.label(
            default = Label("//bdl"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _aggregate_library_providers_impl(ctx):
    providers = [
        ctx.attr.bdl[BdlInfo],
    ]
    for fmt, rule in ctx.attr.generators.items():
        for provider in extensions[fmt]["library"]["providers"]:
            providers.append(rule[provider])

    return providers

_aggregate_library_providers = rule(
    implementation = _aggregate_library_providers_impl,
    doc = """Aggregates the providers from the various language specific generators.""",
    attrs = {
        "bdl": attr.label(
            mandatory = True,
            providers = [BdlInfo],
            doc = "Bdl precompiler rule.",
        ),
        "generators": attr.string_keyed_label_dict(
            mandatory = True,
            doc = "Generator identifiers to generator rules.",
        ),
    },
)

def _bdl_library_impl(name, visibility, srcs, deps, implementation, **kwargs):
    _bdl_precompile_library(
        name = "{}.precompile".format(name),
        srcs = srcs,
        deps = deps,
        **kwargs
    )

    generators = {}
    for fmt, extension in extensions.items():
        if "library" in extension:
            generators[fmt] = "{}.{}".format(name, fmt)
            extension["library"]["generator"](
                name = generators[fmt],
                visibility = visibility,
                bdl = "{}.precompile".format(name),
                srcs = srcs,
                deps = deps + ([implementation[fmt]] if fmt in implementation else []),
            )

    _aggregate_library_providers(
        name = name,
        bdl = "{}.precompile".format(name),
        generators = generators,
        visibility = visibility,
    )

bdl_library = macro(
    doc = "Bdl library with language specific generators.",
    implementation = _bdl_library_impl,
    inherit_attrs = _bdl_precompile_library,
    attrs = {
        "deps": attr.label_list(
            providers = [BdlInfo],
            doc = "List of bdl dependencies.",
        ),
        "implementation": attr.string_keyed_label_dict(
            doc = "Implementation for a specific language, will be added to generated code for the given language.",
            configurable = False,
        ),
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of Bzd Description Language (bdl) files to be included.",
            configurable = False,
        ),
    },
)
