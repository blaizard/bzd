"""NodeJs package rule.

Each package is a directory of files (a `DirectoryInfo`). The install rule copies those files into a
tree artifact and recreates the dependency links as declared symlinks (see
`bzd_nodejs_make_node_modules`), so that no tree artifact contains a symlink.
"""

load("@bazel_skylib//rules/directory:providers.bzl", "DirectoryInfo")

# ---- Providers

BzdNodeJsPackageInfo = provider(
    "Provider for a single package",
    fields = {
        "canonical_name": "The canonical name of the package, ex: foo@1.0.0",
        "deps": "Dictionary of dependency alias to canonical name.",
        "module_name": "The module name of the package, ex: foo",
        "srcs": "The files of the package.",
        "transitive_infos": "All transitive package infos (excluding itself).",
    },
)

# ---- Rule

def _bzd_nodejs_package_impl(ctx):
    srcs = ctx.attr.srcs[DirectoryInfo]

    # Direct dependencies with their link alias, which may differ from the dependency module name
    # (npm aliases). This cannot be derived from `transitive_infos`, which only carries the closure.
    deps = {
        module_name: dep[BzdNodeJsPackageInfo].canonical_name
        for module_name, dep in ctx.attr.deps.items()
    }
    dep_infos = [dep[BzdNodeJsPackageInfo] for dep in ctx.attr.deps.values()]

    return [DefaultInfo(files = srcs.transitive_files), BzdNodeJsPackageInfo(
        canonical_name = ctx.attr.canonical_name,
        deps = deps,
        module_name = ctx.attr.module_name,
        srcs = srcs,
        transitive_infos = depset(dep_infos, transitive = [info.transitive_infos for info in dep_infos]),
    )]

bzd_nodejs_package = rule(
    doc = "Package implementation.",
    implementation = _bzd_nodejs_package_impl,
    attrs = {
        "canonical_name": attr.string(
            mandatory = True,
            doc = "The canonical name of the package.",
        ),
        "deps": attr.string_keyed_label_dict(
            providers = [BzdNodeJsPackageInfo],
            doc = "The dependencies associated with this package.",
        ),
        "module_name": attr.string(
            mandatory = True,
            doc = "The module name of the package.",
        ),
        "srcs": attr.label(
            mandatory = True,
            doc = "The source files of the package.",
            providers = [DirectoryInfo],
        ),
    },
    provides = [BzdNodeJsPackageInfo],
)
