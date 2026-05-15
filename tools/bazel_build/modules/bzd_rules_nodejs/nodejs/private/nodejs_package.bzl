"""NodeJs package rule.

Source: https://pnpm.io/symlinked-node-modules-structure

The target is to recreate such structure:

node_modules
├── foo -> ./.pnpm/foo@1.0.0/node_modules/foo
└── .pnpm
    ├── bar@1.0.0
    │   └── node_modules
    │       ├── bar -> <store>
    │       └── qar -> ../../qar@2.0.0/node_modules/qar
    ├── foo@1.0.0
    │   └── node_modules
    │       ├── foo -> <store>
    │       ├── bar -> ../../bar@1.0.0/node_modules/bar
    │       └── qar -> ../../qar@2.0.0/node_modules/qar
    └── qar@2.0.0
        └── node_modules
            └── qar -> <store>
"""

# ---- Providers

BzdNodeJsPackageInfo = provider(
    "Provider for a single package",
    fields = {
        "archive": "The archive of this package.",
        "canonical_name": "The canonical name of the package, ex: foo@1.0.0",
        "module_name": "The module name of the package, ex: foo",
        "source_path": "Where the source of this package should be extracted.",
        "symlink": "The top level symlink for this package.",
    },
)

BzdNodeJsPackagesInfo = provider(
    "Provider for packages",
    fields = {
        "transitive_packages": "All packages related to this package (including itself).",
        "transitive_symlinks": "All symlinks related to the dependencies of this package.",
    },
)

# ---- Rule

def _bzd_nodejs_package_impl(ctx):
    package_node_modules = "node_modules/.store/{}/node_modules".format(ctx.attr.canonical_name)

    # Compute the dependencies symlinks.
    symlinks = []
    for module_name, dep in ctx.attr.deps.items():
        symlinks.append(("{}/{}".format(package_node_modules, module_name), dep[BzdNodeJsPackageInfo].canonical_name))

    package = BzdNodeJsPackageInfo(
        canonical_name = ctx.attr.canonical_name,
        module_name = ctx.attr.module_name,
        archive = ctx.file.archive,
        source_path = "{}/{}".format(package_node_modules, ctx.attr.module_name),
        symlink = ("node_modules/{}".format(ctx.attr.module_name), ctx.attr.canonical_name),
    )

    return [package, BzdNodeJsPackagesInfo(
        transitive_packages = depset([package], transitive = [dep[BzdNodeJsPackagesInfo].transitive_packages for dep in ctx.attr.deps.values()]),
        transitive_symlinks = depset(symlinks, transitive = [dep[BzdNodeJsPackagesInfo].transitive_symlinks for dep in ctx.attr.deps.values()]),
    )]

bzd_nodejs_package = rule(
    doc = "Package implementation.",
    implementation = _bzd_nodejs_package_impl,
    attrs = {
        "archive": attr.label(
            mandatory = True,
            allow_single_file = [".tar.gz", ".tgz"],
            doc = "The package archive.",
        ),
        "canonical_name": attr.string(
            mandatory = True,
            doc = "The canonical name of the package.",
        ),
        "deps": attr.string_keyed_label_dict(
            providers = [BzdNodeJsPackagesInfo, BzdNodeJsPackageInfo],
            doc = "The dependencies associated with this package.",
        ),
        "module_name": attr.string(
            mandatory = True,
            doc = "The module name of the package.",
        ),
    },
    provides = [BzdNodeJsPackageInfo],
)
