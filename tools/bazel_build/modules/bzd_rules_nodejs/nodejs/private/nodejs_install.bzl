"""NodeJs install rule."""

load("//nodejs:private/nodejs_library.bzl", "BzdNodeJsDepsInfo", "bzd_nodejs_setup")
load("//nodejs:private/nodejs_package.bzl", "BzdNodeJsPackageInfo")

# ---- Provider

BzdNodeJsInstallInfo = provider(
    "Provider for installation information.",
    fields = {
        "api": "The interface metadata.",
        "file_mapping": "A mapping between the original file and the transpiled file.",
        "files": "All the files of the installation.",
        "modules": "Top level module names, will be used for deduplicating.",
        "node_modules": "The node_modules directory.",
        "package_json": "The package.json file located at the root.",
    },
)

def _bzd_nodejs_install_impl(ctx):
    provider = ctx.attr.library[BzdNodeJsDepsInfo]
    base_dir_name = ctx.label.name

    # --- Setup the nodejs environment

    packages = provider.packages.to_list()
    setup = bzd_nodejs_setup(
        ctx = ctx,
        file_locations = provider.file_locations,
        packages = packages,
        apis = provider.apis,
        base_dir_name = base_dir_name,
    )

    # --- Fill in the metadata

    metadata = ctx.actions.declare_file("{}.nodejs_install/metadata.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [setup.package_json, setup.api, setup.node_modules],
        outputs = [metadata],
        progress_message = "Generating manifest for {}".format(ctx.label),
        mnemonic = "NodejsMetadata",
        arguments = [
            "--package_json",
            setup.package_json.path,
            metadata.path,
        ],
        executable = ctx.attr._metadata.files_to_run,
    )

    # Return the providers (including outputs and dependencies)
    return [
        BzdNodeJsInstallInfo(
            api = setup.api,
            files = depset([setup.package_json, setup.api, setup.node_modules] + setup.file_mapping.values(), transitive = [provider.data]),
            package_json = setup.package_json,
            node_modules = setup.node_modules,
            file_mapping = setup.file_mapping,
            modules = [package[BzdNodeJsPackageInfo].module_name for package in packages],
        ),
    ]

bzd_nodejs_install = rule(
    doc = """
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
""",
    implementation = _bzd_nodejs_install_impl,
    attrs = {
        "library": attr.label(
            doc = "Install the given library.",
            mandatory = True,
            providers = [BzdNodeJsDepsInfo],
        ),
        "_json_merge": attr.label(
            default = Label("@bzd_lib//:json_merge"),
            cfg = "exec",
            executable = True,
        ),
        "_metadata": attr.label(
            default = Label("//nodejs/metadata"),
            cfg = "exec",
            executable = True,
        ),
        "_node_modules": attr.label(
            default = "//nodejs/private/python:node_modules",
            doc = "Setup the node_modules directory.",
            cfg = "exec",
            executable = True,
        ),
    },
)
