"""Utility for NodeJs rules."""

# ---- Providers

BzdNodeJsDepsInfo = provider("Provider for dependencies information", fields = ["packages", "srcs", "data"])

# ---- Utils

def bzd_nodejs_make_provider(ctx):
    """Create a provider from a rule context."""

    return BzdNodeJsDepsInfo(
        srcs = depset(ctx.files.srcs),
        data = depset(ctx.files.data),
        packages = dict(ctx.attr.packages),
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

# ---- Aspect

def _bzd_nodejs_deps_aspect_impl(_target, ctx):
    if ctx.rule.kind == "bzd_nodejs_library":
        return []
    provider = bzd_nodejs_merge(*[d[BzdNodeJsDepsInfo] for d in ctx.rule.attr.deps])
    return [provider]

bzd_nodejs_deps_aspect = aspect(
    doc = "Aspects to gather data from bzd depedencies.",
    implementation = _bzd_nodejs_deps_aspect_impl,
    attr_aspects = ["deps"],
)
