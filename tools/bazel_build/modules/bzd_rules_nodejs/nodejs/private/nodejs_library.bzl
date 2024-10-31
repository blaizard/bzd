"""NodeJs library rule."""

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

BzdNodeJsPackageInfo = provider(
    "Provider for node packages",
    fields = {
        "package": "The package name.",
        "packages": "Overrides for the package names and their corresponding tarball files.",
        "version": "The package version.",
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

def _bzd_nodejs_package_impl(ctx):
    return BzdNodeJsPackageInfo(
        package = ctx.attr.package,
        version = ctx.attr.version,
        packages = {name: target[DefaultInfo].files.to_list()[0] for name, target in ctx.attr.packages.items()},
    )

bzd_nodejs_package = rule(
    doc = "Package implementation.",
    implementation = _bzd_nodejs_package_impl,
    attrs = {
        "package": attr.string(
            mandatory = True,
            doc = "The package name.",
        ),
        "packages": attr.string_keyed_label_dict(
            allow_files = [".tgz"],
            doc = "Overrides for the package names and their corresponding tarball files.",
        ),
        "version": attr.string(
            mandatory = True,
            doc = "The package version.",
        ),
    },
    provides = [BzdNodeJsPackageInfo],
)
