"""NodeJs library rule."""

load("//nodejs:private/nodejs_package.bzl", "BzdNodeJsPackageInfo")

# ---- Providers

BzdNodeJsDepsInfo = provider(
    "Provider for dependencies information",
    fields = {
        "apis": "APIs files that these deps implements.",
        "data": "Data to be added at runtime.",
        "packages": "Packages to be used.",
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
        packages = depset(ctx.attr.packages),
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
        packages = depset(transitive = [p.packages for p in providers]),
    )

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
        providers = [BzdNodeJsDepsInfo],
    ),
    "packages": attr.label_list(
        doc = "Package dependencies.",
        providers = [BzdNodeJsPackageInfo],
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
    return provider

bzd_nodejs_library = rule(
    doc = "A library contains all dependencies used for this target.",
    implementation = _bzd_nodejs_library_impl,
    attrs = LIBRARY_ATTRS,
    provides = [BzdNodeJsDepsInfo],
)
