"""NodeJs package rule.

The target is to recreate such structure:

node_modules
├── foo -> ./.store/foo@1.0.0
└── .store
    ├── bar@1.0.0
    │   ├── <src>
    │   └── node_modules
    │       └── qar -> ../../qar@2.0.0
    ├── foo@1.0.0
    │   ├── <src>
    │   └── node_modules
    │       ├── bar -> ../../bar@1.0.0
    │       └── qar -> ../../qar@2.0.0
    └── qar@2.0.0
        └── <src>
"""

# ---- Providers

BzdNodeJsPackageInfo = provider(
    "Provider for a single package",
    fields = {
        "canonical_name": "The canonical name of the package, ex: foo@1.0.0",
        "module_name": "The module name of the package, ex: foo",
        "transitive_stores": "All stores archive related to this package (including itself).",
    },
)

# ---- Rule

def _bzd_nodejs_package_impl(ctx):
    store = ctx.actions.declare_file("store.tar")
    args = ctx.actions.args()
    args.add("--output", store)
    args.add("--root", ctx.attr.canonical_name)

    # Compute the symlinks and their relative paths.
    node_modules = "{}/node_modules".format(ctx.attr.canonical_name)
    for module_name, dep in ctx.attr.deps.items():
        symlink = "{}/{}".format(node_modules, module_name)
        path_up = [".."] * (len(symlink.split("/")) - 1)
        relative_path = "/".join(path_up + [dep[BzdNodeJsPackageInfo].canonical_name])
        args.add_all("--symlink", [module_name, relative_path])

    args.add(ctx.file.archive)

    ctx.actions.run(
        inputs = [ctx.file.archive],
        outputs = [store],
        arguments = [args],
        progress_message = "Repackaging {} for {}".format(ctx.attr.canonical_name, ctx.label),
        mnemonic = "NodeJsRepackage",
        executable = ctx.executable._repackage,
    )

    return BzdNodeJsPackageInfo(
        canonical_name = ctx.attr.canonical_name,
        module_name = ctx.attr.module_name,
        transitive_stores = depset([store], transitive = [dep[BzdNodeJsPackageInfo].transitive_stores for dep in ctx.attr.deps.values()]),
    )

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
            providers = [BzdNodeJsPackageInfo],
            doc = "The dependencies associated with this package.",
        ),
        "module_name": attr.string(
            mandatory = True,
            doc = "The module name of the package.",
        ),
        "_repackage": attr.label(
            default = "//nodejs/private/python:repackage",
            doc = "Repackage npm packages to include relative symlinks.",
            cfg = "exec",
            executable = True,
        ),
    },
    provides = [BzdNodeJsPackageInfo],
)
