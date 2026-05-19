"""NodeJs install rule."""

load("@bzd_package//:defs.bzl", "BzdPackageMetadataFragmentInfo")
load("//nodejs:private/nodejs_library.bzl", "LIBRARY_ATTRS", "bzd_nodejs_library_get_provider")
load("//nodejs:private/nodejs_package.bzl", "BzdNodeJsPackageInfo", "BzdNodeJsPackagesInfo")

# ---- Provider

BzdNodeJsInstallInfo = provider(
    "Provider for installation information.",
    fields = {
        "api": "The interface metadata.",
        "files": "All the files of the installation (extraction outputs, sources, transpiled files).",
        "package_json": "The package.json file located at the root.",
        "symlink_targets": "A map of symlink path to target extraction output for runfiles.",
        "transpiled": "The map of transpiled files.",
    },
)

# ---- Helpers

def _relpath(from_file, to_file):
    """Compute the relative filesystem path from from_file's parent directory to to_file.

    This produces a relative path string suitable for use as target_path in ctx.actions.symlink().
    The path is relative to the parent directory of from_file.
    """
    from_parts = from_file.path.split("/")
    to_parts = to_file.path.split("/")

    # from_file's parent directory (remove the filename component)
    from_parts = from_parts[:-1]

    # Find the common prefix length
    common = 0
    for a, b in zip(from_parts, to_parts):
        if a == b:
            common += 1
        else:
            break

    # Go up from from_parts to common ancestor
    up = [".."] * (len(from_parts) - common)

    # Go down from common ancestor to to_parts
    down = to_parts[common:]

    return "/".join(up + down)

def bzd_nodejs_add_symlink_runfiles(ctx, runfiles, symlink_targets, prefix = None):
    """Add symlink target entries to runfiles.

    The declare_symlink artifacts cannot be passed through runfiles in Bazel 9.1.0
    (NullPointerException in SourceManifestAction for symlinks mapping).
    Instead, we map the symlink paths directly to the extraction target
    TreeArtifacts, avoiding the crash while still providing the correct
    node_modules structure at runtime.

    Args:
        ctx: The rule context.
        runfiles: The runfiles object to merge into.
        symlink_targets: Dict mapping symlink path to extraction TreeArtifact.
        prefix: Optional path prefix to prepend to each symlink entry.

    Returns:
        The merged runfiles object.
    """
    for symlink_path, target_file in symlink_targets.items():
        if prefix:
            symlink_path = prefix + "/" + symlink_path
        runfiles = runfiles.merge(ctx.runfiles(symlinks = {symlink_path: target_file}))
    return runfiles

# ---- Public API

def bzd_nodejs_make_node_modules(ctx, packages, base_dir_name):
    """Generate a node_modules and a package.json file at the root of `base_dir_name`.

    Args:
        ctx: The context of the rule.
        packages: The packages to be installed.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        A struct with:
          - package_json: The package.json file.
          - source_dirs: A dict mapping canonical name to extraction TreeArtifact.
          - symlinks: A list of declare_symlink artifacts (for use as action inputs).
          - symlink_targets: A dict mapping symlink path to target extraction output
            (for use in runfiles, avoids Bazel NPE with declare_symlink artifacts).
    """

    # Outputs of this rule.
    package_json = ctx.actions.declare_file("{}/package.json".format(base_dir_name))

    package_json_content = {
        "imports": {
            "#bzd/*": "./*",
        },
        "license": "UNLICENSED",
        "name": "{}.{}".format(ctx.label.package.replace("/", "."), ctx.attr.name),
        "private": True,
        "type": "module",
        "version": "0.0.0",
    }

    ctx.actions.write(
        output = package_json,
        content = json.encode_indent(package_json_content),
    )

    transitive_packages = depset(transitive = [package[BzdNodeJsPackagesInfo].transitive_packages for package in packages])
    transitive_symlinks = depset([package[BzdNodeJsPackageInfo].symlink for package in packages], transitive = [package[BzdNodeJsPackagesInfo].transitive_symlinks for package in packages])

    # Extract package sources.
    sources = {}
    for package in transitive_packages.to_list():
        package_sources = ctx.actions.declare_directory("{}/{}".format(base_dir_name, package.source_path))
        args = ctx.actions.args()
        args.add("--output", package_sources.path)
        args.add(package.archive)

        # Note, this action should be performed only once and cache for other node_modules as it only depends on package-related inputs.
        ctx.actions.run(
            inputs = [package.archive],
            outputs = [package_sources],
            arguments = [args],
            progress_message = "Extracting {} for {}".format(package.canonical_name, ctx.label),
            mnemonic = "NodejsExtract",
            executable = ctx.executable._extract,
        )

        if package.canonical_name in sources:
            fail("The module '{}' was extracted twice, it should never happen.".format(package.canonical_name))
        sources[package.canonical_name] = package_sources

    # Assemble the pnpm-like node_modules with relative symlinks.
    #
    # Uses declare_symlink + target_path with a computed relative path so
    # symlinks are portable across machines (e.g., under --remote_download_minimal).
    # The declare_symlink artifacts are NOT passed through runfiles directly
    # (they cause a NullPointerException in Bazel 9.1.0's SourceManifestAction).
    # Instead, symlink paths are mapped to their extraction targets via
    # symlink_targets for runfiles creation in the caller.
    symlinks = []
    symlink_targets = {}
    for path, module in transitive_symlinks.to_list():
        symlink = ctx.actions.declare_symlink("{}/{}".format(base_dir_name, path))
        if module not in sources:
            fail("The module '{}' was not extracted, it should never happen.".format(module))
        target = sources[module]
        ctx.actions.symlink(
            output = symlink,
            target_path = _relpath(symlink, target),
        )
        symlinks.append(symlink)
        symlink_targets[path] = target

    return struct(
        package_json = package_json,
        source_dirs = sources,
        symlinks = symlinks,
        symlink_targets = symlink_targets,
    )

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
    to_copy = {}
    for f in srcs:
        # This makes all file live at the same level.
        copy = ctx.actions.declare_file("{}/{}".format(base_dir_name, f.short_path.replace("../", "external/")))
        generated.append(copy)
        to_copy[f.path] = copy.path

    # Copy all sources, otherwise (if symlinked) node will use their real ath to resolve imports.
    if to_copy:
        ctx.actions.run_shell(
            inputs = srcs,
            outputs = generated,
            command = "\n".join(["cp \"{}\" \"{}\"".format(src, dst) for src, dst in to_copy.items()]),
            mnemonic = "CopyFiles",
            progress_message = "Copying {} source file(s) for {}".format(len(to_copy), ctx.label),
        )

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

# ---- Rule implementation

def _bzd_nodejs_install_impl(ctx):
    providers = bzd_nodejs_library_get_provider(ctx)
    base_dir_name = ctx.label.name

    # --- Generate the package.json and node_modules files

    result = bzd_nodejs_make_node_modules(ctx, providers.packages.to_list(), base_dir_name = base_dir_name)
    package_json = result.package_json
    node_modules = result.source_dirs.values() + result.symlinks

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

    srcs, transpiled = bzd_nodejs_transpile(ctx, providers.srcs.to_list(), runfiles = [package_json] + node_modules, base_dir_name = base_dir_name)

    # --- Fill in the metadata

    metadata = ctx.actions.declare_file("{}.nodejs_install/metadata.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [package_json, api] + node_modules,
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

    # Return the providers (including outputs and dependencies).
    #
    # NOTE: declare_symlink artifacts are NOT included in the main files depset
    # because they cause a NullPointerException in Bazel 9.1.0's
    # SourceManifestAction when passed through runfiles. They are included in
    # OutputGroupInfo to ensure they are materialized in the output tree,
    # and symlink paths are mapped to extraction targets via symlink_targets
    # for runfiles creation in the caller.
    return [
        BzdPackageMetadataFragmentInfo(manifests = [metadata]),
        BzdNodeJsInstallInfo(
            api = api,
            files = depset([package_json, api] + result.source_dirs.values() + srcs + transpiled.values(), transitive = [providers.data]),
            package_json = package_json,
            transpiled = transpiled,
            symlink_targets = result.symlink_targets,
        ),
        DefaultInfo(
            files = depset([package_json, api] + result.source_dirs.values() + srcs + transpiled.values() + result.symlinks, transitive = [providers.data]),
        ),
        OutputGroupInfo(
            nodejs_symlinks = depset(result.symlinks),
        ),
    ]

bzd_nodejs_install = rule(
    doc = """
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
""",
    implementation = _bzd_nodejs_install_impl,
    attrs = {
        "_extract": attr.label(
            default = "//nodejs/private/python:extract",
            doc = "The extract binary.",
            cfg = "exec",
            executable = True,
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
)
