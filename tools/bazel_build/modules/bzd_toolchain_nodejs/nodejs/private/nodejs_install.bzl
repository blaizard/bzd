"""NodeJs install rule."""

load("@bzd_package//:defs.bzl", "BzdPackageMetadataFragmentInfo")
load("@bzd_toolchain_nodejs//nodejs:private/utils.bzl", "BzdNodeJsDepsInfo", "BzdNodeJsInstallInfo", "bzd_nodejs_deps_aspect", "bzd_nodejs_make_provider", "bzd_nodejs_merge")

COMMON_ATTRS = {
    "data": attr.label_list(
        allow_files = True,
        doc = "Data to be added to the runfile list",
    ),
    "deps": attr.label_list(
        aspects = [bzd_nodejs_deps_aspect],
        allow_files = True,
        doc = "Dependencies",
    ),
    "packages": attr.string_dict(
        allow_empty = True,
        doc = "Package dependencies",
    ),
    "srcs": attr.label_list(
        allow_files = True,
        doc = "Source files",
    ),
    "tools": attr.label_list(
        doc = "Additional tools to be added to the runfile",
        cfg = "target",
    ),
}

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
	"version": "1.0.0",
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

    manager_args = ["--dir", package_json.dirname, "install", "--fetch-timeout=300000000", "--silent", "--ignore-scripts", "--prefer-offline"]

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["@bzd_toolchain_nodejs//nodejs:toolchain_type"].executable

    ctx.actions.run(
        inputs = [package_json],
        outputs = [node_modules],
        progress_message = "Updating package(s) for {}".format(ctx.label),
        arguments = manager_args,
        mnemonic = "NodejsUpdate",
        executable = toolchain_executable.manager.files_to_run,
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
        )

    return generated, typescript

def _bzd_nodejs_install_impl(ctx):
    rule_provider = bzd_nodejs_make_provider(ctx)
    deps_provider = bzd_nodejs_merge(rule_provider, *[d[BzdNodeJsDepsInfo] for d in ctx.attr.deps])
    base_dir_name = ctx.label.name

    # --- Generate the package.json and node_modules files

    package_json, node_modules = bzd_nodejs_make_node_modules(ctx, deps_provider.packages, base_dir_name = base_dir_name)

    # --- Apply transpilers to the sources

    srcs, transpiled = bzd_nodejs_transpile(ctx, deps_provider.srcs.to_list(), runfiles = [package_json, node_modules], base_dir_name = base_dir_name)

    # --- Build the runfiles for the execution environment

    #print(deps_provider.data.to_list())
    files = [package_json, node_modules] + srcs + transpiled.values() + deps_provider.data.to_list()
    runfiles = ctx.runfiles(
        symlinks = {f.short_path.removeprefix(ctx.label.package + "/" + base_dir_name + "/"): f for f in files},
    )

    # --- Fill in the metadata

    metadata = ctx.actions.declare_file("{}.nodejs_install/metadata.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [package_json, node_modules],
        outputs = [metadata],
        progress_message = "Generating manifest for {}".format(ctx.label),
        mnemonic = "NodejsMetadata",
        arguments = [
            "--package_json",
            package_json.path,
            metadata.path,
        ],
        executable = ctx.attr._metadata.files_to_run,
    )

    # Return the provides (including outputs and dependencies)
    return [
        DefaultInfo(default_runfiles = runfiles, files = depset([package_json, node_modules])),
        BzdPackageMetadataFragmentInfo(manifests = [metadata]),
        BzdNodeJsInstallInfo(
            files = files,
            root_dir = base_dir_name,
            transpiled = transpiled,
            runfiles = runfiles,
        ),
    ]

_INSTALL_ATTRS = dict(COMMON_ATTRS)
_INSTALL_ATTRS.update({
    "_cache": attr.label(
        default = "@bzd_toolchain_nodejs//nodejs:cache",
    ),
    "_metadata": attr.label(
        default = Label("@bzd_toolchain_nodejs//nodejs/metadata"),
        cfg = "exec",
        executable = True,
    ),
    "_tsc": attr.label(
        default = Label("@bzd_toolchain_nodejs//typescript:tsc"),
        cfg = "exec",
        executable = True,
    ),
    "_tsconfig": attr.label(
        default = Label("@bzd_toolchain_nodejs//typescript:tsconfig.json"),
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
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
)
