"""NodeJs package rule."""

# ---- Providers

BzdNodeJsPackageInfo = provider(
    "Provider for node packages",
    fields = {
        "dependencies": "A JSON file containing the dependencies graph of this package.",
        "packages": "Overrides for the package names and their corresponding tarball files.",
    },
)

# ---- Rule

def _bzd_nodejs_package_impl(ctx):
    return BzdNodeJsPackageInfo(
        dependencies = ctx.file.dependencies,
        packages = {name: target[DefaultInfo].files.to_list()[0] for name, target in ctx.attr.packages.items()},
    )

bzd_nodejs_package = rule(
    doc = "Package implementation.",
    implementation = _bzd_nodejs_package_impl,
    attrs = {
        "dependencies": attr.label(
            mandatory = True,
            allow_single_file = [".json"],
            doc = "A JSON file containing the dependencies graph of this package.",
        ),
        "packages": attr.string_keyed_label_dict(
            allow_files = [".tgz"],
            doc = "Overrides for the package names and their corresponding tarball files.",
        ),
    },
    provides = [BzdNodeJsPackageInfo],
)
