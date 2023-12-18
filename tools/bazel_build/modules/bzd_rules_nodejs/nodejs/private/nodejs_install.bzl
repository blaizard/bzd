"""NodeJs install rule."""

load("@bzd_package//:defs.bzl", "BzdPackageMetadataFragmentInfo")
load("@bzd_rules_nodejs//nodejs:private/nodejs_library.bzl", "LIBRARY_ATTRS", "bzd_nodejs_library_get_provider")

# ---- Provider

BzdNodeJsInstallInfo = provider(
    "Provider for installation information.",
    fields = {
        "files": "All the files of the installation.",
        "package_json": "The package.json file located at the root.",
        "transpiled": "The map of transpiled files.",
    },
)

def bzd_nodejs_make_node_modules(ctx, packages, base_dir_name):
    """Generate a node_modules and a package.json file at the root of `base_dir_name`.

    Args:
        ctx: The context of the rule.
        packages: The packages to be installed.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        A tupple containing the package.json file and the node_modules directory populated.
    """

    # Outputs of this rule.
    package_json = ctx.actions.declare_file("{}/package.json".format(base_dir_name))
    node_modules = ctx.actions.declare_directory("{}/node_modules".format(base_dir_name))

    # Build the dependencies template replacement string
    dependencies = ",\n".join(["\"{}\": \"{}\"".format(name, version if version else "latest") for name, version in packages.items()])

    ctx.actions.write(
        output = package_json,
        content = """
{{
	"name": "{name}",
	"version": "0.0.0",
	"license": "UNLICENSED",
	"private": true,
	"type": "module",
    "imports": {{
        "#bzd/*": "./*"
    }},
	"dependencies": {{
		{dependencies}
	}}
}}
""".format(
            dependencies = dependencies,
            name = "{}.{}".format(ctx.label.package.replace("/", "."), ctx.attr.name),
        ),
    )

    # --- Fetch and install the packages

    manager_args = [
        "--dir",
        package_json.dirname,
        "install",
        "--fetch-timeout=300000000",
        "--ignore-scripts",
        "--prefer-offline",
        "--silent",
    ]

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["@bzd_rules_nodejs//nodejs:toolchain_type"].executable

    ctx.actions.run(
        inputs = [package_json],
        outputs = [node_modules],
        progress_message = "Updating package(s) for {}".format(ctx.label),
        arguments = manager_args,
        mnemonic = "NodejsUpdate",
        executable = toolchain_executable.manager.files_to_run,
        # Because of symlinks to the node_modules folder, this should not run remotely.
        execution_requirements = {"no-remote": "1"},
    )

    return [package_json, node_modules]

def bzd_nodejs_transpile(ctx, srcs, runfiles, base_dir_name):
    """Build a file tree at the root of `base_dir` and transpile the files if needed.

    Args:
        ctx: The context of the rule.
        srcs: The source files.
        runfiles: Additional files to be included while transpiling.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        A tupple containing the generated files and the transpiled files as a dictionary.
    """

    base_dir_short_path = ctx.label.package + "/" + base_dir_name
    base_dir_path = ctx.genfiles_dir.path + "/" + base_dir_short_path

    # Map all the sources to the generated files directory.
    generated = []
    for f in srcs:
        symlink = ctx.actions.declare_file("{}/{}".format(base_dir_name, f.short_path))
        ctx.actions.symlink(
            output = symlink,
            target_file = f,
        )
        generated.append(symlink)

    # Convert TypeScript to Javascript
    typescript = {}
    for f in generated:
        if f.path.endswith(".ts"):
            path = f.short_path.removeprefix(base_dir_short_path + "/")
            expected = ctx.actions.declare_file(f.basename.replace(".ts", ".js"), sibling = f)
            typescript[path] = expected

    # If there are any typescript files to process...
    if typescript:
        tsconfig = ctx.actions.declare_file("{}/tsconfig.json".format(base_dir_name))
        ctx.actions.symlink(
            output = tsconfig,
            target_file = ctx.file._tsconfig,
        )

        ctx.actions.run(
            inputs = generated + runfiles + [tsconfig],
            outputs = typescript.values(),
            progress_message = "Processing TypeScript for {}...".format(ctx.label),
            arguments = [
                "--baseUrl",
                base_dir_path,
                "--project",
                base_dir_path,
            ],
            executable = ctx.executable._tsc,
            execution_requirements = {"no-remote": "1"},
        )

    return generated + typescript.values(), {original: final.short_path.removeprefix(base_dir_short_path + "/") for original, final in typescript.items()}

def _bzd_nodejs_install_impl(ctx):
    providers = bzd_nodejs_library_get_provider(ctx)
    base_dir_name = ctx.label.name

    # --- Generate the package.json and node_modules files

    package_json, node_modules = bzd_nodejs_make_node_modules(ctx, providers.packages, base_dir_name = base_dir_name)

    # --- Create the APIs

    api = ctx.actions.declare_file("api.json", sibling = package_json)
    ctx.actions.run(
        inputs = providers.apis,
        outputs = [api],
        progress_message = "Generating API for {}...".format(ctx.label),
        arguments = [
            "--output",
            api.path,
        ] + [f.path for f in providers.apis.to_list()],
        executable = ctx.executable._json_merge,
    )

    # --- Apply transpilers to the sources

    srcs, transpiled = bzd_nodejs_transpile(ctx, providers.srcs.to_list(), runfiles = [package_json, node_modules], base_dir_name = base_dir_name)

    # --- Fill in the metadata

    metadata = ctx.actions.declare_file("{}.nodejs_install/metadata.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [package_json, node_modules, api],
        outputs = [metadata],
        progress_message = "Generating manifest for {}".format(ctx.label),
        mnemonic = "NodejsMetadata",
        arguments = [
            "--package_json",
            package_json.path,
            metadata.path,
        ],
        executable = ctx.attr._metadata.files_to_run,
        execution_requirements = {"no-remote": "1"},
    )

    # Return the providers (including outputs and dependencies)
    return [
        BzdPackageMetadataFragmentInfo(manifests = [metadata]),
        BzdNodeJsInstallInfo(
            files = depset([package_json, node_modules, api] + srcs + transpiled.values(), transitive = [providers.data]),
            package_json = package_json,
            transpiled = transpiled,
        ),
    ]

_INSTALL_ATTRS = dict(LIBRARY_ATTRS)
_INSTALL_ATTRS.update({
    "_json_merge": attr.label(
        default = Label("@bzd_lib//:json_merge"),
        cfg = "exec",
        executable = True,
    ),
    "_metadata": attr.label(
        default = Label("@bzd_rules_nodejs//nodejs/metadata"),
        cfg = "exec",
        executable = True,
    ),
    "_tsc": attr.label(
        default = Label("@bzd_rules_nodejs//toolchain/typescript:tsc"),
        cfg = "exec",
        executable = True,
    ),
    "_tsconfig": attr.label(
        default = Label("@bzd_rules_nodejs//toolchain/typescript:tsconfig.json"),
        allow_single_file = True,
    ),
})

bzd_nodejs_install = rule(
    doc = """
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
""",
    implementation = _bzd_nodejs_install_impl,
    attrs = _INSTALL_ATTRS,
    toolchains = ["@bzd_rules_nodejs//nodejs:toolchain_type"],
)
