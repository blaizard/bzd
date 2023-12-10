"""NodeJs library rule."""

# ---- Providers

BzdNodeJsDepsInfo = provider(
    "Provider for dependencies information",
    fields = {
        "apis": "APIs files that these deps implements.",
        "data": "Data to be added at runtime.",
        "packages": "Dictionary of packages and their corresponding version.",
        "srcs": "Sources to be processed.",
    },
)

# ---- Utils

def bzd_nodejs_make_provider(ctx):
    """Create a provider from a rule context."""

    tool_depsets = [tool[DefaultInfo].default_runfiles.files for tool in ctx.attr.tools]
    return BzdNodeJsDepsInfo(
        srcs = depset(ctx.files.srcs),
        data = depset(ctx.files.data, transitive = tool_depsets),
        packages = dict(ctx.attr.packages),
        apis = depset(ctx.files.apis),
    )

def bzd_nodejs_merge(*providers):
    """Merge providers of types BzdNodeJsDepsInfo together.

    Args:
        *providers: The providers to be merged.

    Returns:
        A BzdNodeJsDepsInfo provider containing the merged providers.
    """

    provider = BzdNodeJsDepsInfo(
        srcs = depset(transitive = [p.srcs for p in providers]),
        data = depset(transitive = [p.data for p in providers]),
        apis = depset(transitive = [p.apis for p in providers]),
        packages = {},
    )

    for p in providers:
        # Merge packages
        for name, version in p.packages.items():
            # If the version already stored is different
            if name in provider.packages and provider.packages[name] and version and provider.packages[name] != version:
                fail("Version conflict for package '{}': '{}' vs '{}'".format(name, version, provider.packages[name]))
            provider.packages[name] = version

    return provider

# ---- Attributes

LIBRARY_ATTRS = {
    "apis": attr.label_list(
        allow_files = True,
        doc = "APIs files.",
    ),
    "data": attr.label_list(
        allow_files = True,
        doc = "Data to be available at runtime.",
    ),
    "deps": attr.label_list(
        allow_files = True,
        doc = "Dependencies of this rule.",
    ),
    "packages": attr.string_dict(
        allow_empty = True,
        doc = "Package dependencies.",
    ),
    "srcs": attr.label_list(
        allow_files = True,
        doc = "Source files.",
    ),
    "tools": attr.label_list(
        doc = "Additional tools to be added to the runfile",
        cfg = "target",
    ),
}

# ---- Rule

def bzd_nodejs_library_get_provider(ctx):
    rule_provider = bzd_nodejs_make_provider(ctx)
    return bzd_nodejs_merge(rule_provider, *[d[BzdNodeJsDepsInfo] for d in ctx.attr.deps])

def _bzd_nodejs_library_impl(ctx):
    provider = bzd_nodejs_library_get_provider(ctx)
    return [DefaultInfo(), provider]

bzd_nodejs_library = rule(
    doc = "A library contains all depdencies used for this target.",
    implementation = _bzd_nodejs_library_impl,
    attrs = LIBRARY_ATTRS,
)
