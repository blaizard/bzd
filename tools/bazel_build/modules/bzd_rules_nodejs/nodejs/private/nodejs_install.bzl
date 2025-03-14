"""NodeJs install rule."""

load("@bzd_package//:defs.bzl", "BzdPackageMetadataFragmentInfo")
load("//nodejs:private/nodejs_library.bzl", "LIBRARY_ATTRS", "bzd_nodejs_library_get_provider")
load("//nodejs:private/nodejs_package.bzl", "BzdNodeJsPackageInfo")

# ---- Provider

BzdNodeJsInstallInfo = provider(
    "Provider for installation information.",
    fields = {
        "api": "The interface metadata.",
        "files": "All the files of the installation.",
        "package_json": "The package.json file located at the root.",
        "transpiled": "The map of transpiled files.",
    },
)

def path_clean(path):
    """Clean a path and make it relative by removing any leading '/'.

    Args:
        path: The path to cleanup.

    Returns:
        The cleaned-up path.
    """

    cleaned = []
    for segment in path.split("/"):
        if not segment:
            continue
        if segment == "..":
            if len(segment) == 0:
                fail("Missing ")
            cleaned.pop()
        else:
            cleaned.append(segment)

    return "/".join(cleaned)

def path_relative_to(path, relative_to):
    """Return the path relative to a given directory.

    This assumes that both path start from the same root directory.

    Args:
        path: The path to process.
        relative_to: `path` should be relative to this given path.

    Returns:
        The path made relative to `relative_to`.
    """

    nb_segments_relative_to = len(path_clean(relative_to).split("/"))
    to_root = "/".join([".."] * nb_segments_relative_to)
    return to_root + "/" + path_clean(path)

def bzd_nodejs_make_node_modules(ctx, packages, base_dir_name):
    """Generate a node_modules and a package.json file at the root of `base_dir_name`.

    Args:
        ctx: The context of the rule.
        packages: The packages to be installed.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        A tuple containing the package.json file and the node_modules directory populated.
    """

    # Outputs of this rule.
    package_json = ctx.actions.declare_file("{}/package.json".format(base_dir_name))
    node_modules = ctx.actions.declare_directory("{}/node_modules".format(base_dir_name))

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//nodejs:toolchain_type"].executable

    package_providers = [package[BzdNodeJsPackageInfo] for package in packages]
    overrides = {}
    for package in package_providers:
        overrides |= package.packages

    package_json_content = {
        "dependencies": {package.package: package.version for package in package_providers},
        "imports": {
            "#bzd/*": "./*",
        },
        "license": "UNLICENSED",
        "name": "{}.{}".format(ctx.label.package.replace("/", "."), ctx.attr.name),
        "pnpm": {
            "overrides": {name: path_relative_to(file.path, package_json.dirname) for name, file in overrides.items()},
        },
        "private": True,
        "type": "module",
        "version": "0.0.0",
    }

    ctx.actions.write(
        output = package_json,
        content = json.encode_indent(package_json_content),
    )

    ctx.actions.run(
        inputs = depset([package_json] + overrides.values()),
        outputs = [node_modules],
        arguments = [
            "--dir",
            package_json.dirname,
            "install",
            "--prod",
            "--quiet",  # Note: --slient is better but it hangs: https://github.com/pnpm/pnpm/issues/7839
            "--ignore-scripts",
            "--offline",
        ],
        progress_message = "Installing package(s) for {}".format(ctx.label),
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
        A tuple containing the generated files and the transpiled files as a dictionary.
    """

    base_dir_short_path = ctx.label.package + "/" + base_dir_name
    base_dir_path = ctx.genfiles_dir.path + "/" + base_dir_short_path

    # Map all the sources to the generated files directory.
    generated = []
    for f in srcs:
        # This makes all file live at the same level.
        symlink = ctx.actions.declare_file("{}/{}".format(base_dir_name, f.short_path.replace("../", "external/")))
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

    return generated + typescript.values(), {original: final.short_path.removeprefix(base_dir_short_path + "/") for original, final in typescript.items()}

def _bzd_nodejs_install_impl(ctx):
    providers = bzd_nodejs_library_get_provider(ctx)
    base_dir_name = ctx.label.name

    # --- Generate the package.json and node_modules files

    package_json, node_modules = bzd_nodejs_make_node_modules(ctx, providers.packages.to_list(), base_dir_name = base_dir_name)

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
    )

    # Return the providers (including outputs and dependencies)
    return [
        BzdPackageMetadataFragmentInfo(manifests = [metadata]),
        BzdNodeJsInstallInfo(
            api = api,
            files = depset([package_json, node_modules, api] + srcs + transpiled.values(), transitive = [providers.data]),
            package_json = package_json,
            transpiled = transpiled,
        ),
    ]

bzd_nodejs_install = rule(
    doc = """
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
""",
    implementation = _bzd_nodejs_install_impl,
    attrs = {
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
        "_tsc": attr.label(
            default = Label("//toolchain/typescript:tsc"),
            cfg = "exec",
            executable = True,
        ),
        "_tsconfig": attr.label(
            default = Label("//toolchain/typescript:tsconfig.json"),
            allow_single_file = True,
        ),
    } | LIBRARY_ATTRS,
    toolchains = ["//nodejs:toolchain_type"],
)
