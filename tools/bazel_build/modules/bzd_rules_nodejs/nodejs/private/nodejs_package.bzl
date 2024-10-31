"""NodeJs package rule."""

# ---- Providers

BzdNodeJsPackageInfo = provider(
    "Provider for node packages",
    fields = {
        "package": "The package name.",
        "packages": "Overrides for the package names and their corresponding tarball files.",
        "version": "The package version.",
    },
)

# ---- Rule

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
