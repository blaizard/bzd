"""Package rules."""

BzdPackageFragmentInfo = provider(
    "Provider for a package fragment.",
    fields = {
        "files": "A map of depsets which key corresponds to the remapping prefix.",
    },
)

BzdPackageMetadataFragmentInfo = provider("Provider for metadata framgents.", fields = ["manifests"])

# ---- Helper

def bzd_package_prefix_from_file(f, depth_from_root = 0):
    """Create a prefix to be used for the creation of BzdPackageFragmentInfo.

    Args:
        f: The file to be used as a base.
        depth_from_root: The depth in the directory tree of this file compared to the root.

    Returns:
        The prefix.
    """

    path = f.short_path
    for _ in range(depth_from_root):
        last_pkg = path.rfind("/")
        if last_pkg == -1:
            fail("The file '{}' does not have {} parent(s).".format(f.short_path, depth_from_root))
        path = path[:last_pkg]
    return path + "/"

def bzd_package_to_runfiles(ctx, fragment):
    """Convert a package fragment into runfiles.

    Args:
        ctx: The rule context to be used.
        fragment: The package fragment to be converted.

    Returns:
        The runfiles.
    """

    symlinks = {}
    for prefix, files in fragment.files.items():
        for f in files.to_list():
            path = f.short_path.removeprefix(prefix)
            if path in symlinks:
                fail("Path '{}' conflicts from the package fragments: {} and {}".format(path, symlinks[path], f))
            symlinks[path] = f

    return ctx.runfiles(
        symlinks = symlinks,
    )

# ---- Packages

def _bzd_package_impl(ctx):
    # Create the manifest.
    # This is a json dictionary with the remapped file path as keys and the actual file path as values.
    manifest_data = {}
    inputs = []
    for target, root in ctx.attr.deps.items():
        info = target[BzdPackageFragmentInfo]
        for prefix, files in info.files.items():
            for f in files.to_list():
                manifest_data[root + "/" + f.short_path.removeprefix(prefix)] = f.path
            inputs.append(files)
    manifest = ctx.actions.declare_file("{}.package.manifest".format(ctx.label.name))
    ctx.actions.write(
        output = manifest,
        content = json.encode(manifest_data),
    )

    # Build buildstamp
    buildstamp = ctx.actions.declare_file("{}.buildstamp.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [],
        outputs = [buildstamp],
        progress_message = "Generating buildstamp for {}".format(ctx.label),
        arguments = [buildstamp.path],
        executable = ctx.executable._buildstamp,
    )

    # Merge all metadata fragments together (TODO)
    metadata = ctx.actions.declare_file("{}.metadata.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [buildstamp],
        outputs = [metadata],
        progress_message = "Generating metadata for {}".format(ctx.label),
        arguments = ["--input", "", buildstamp.path, metadata.path],
        executable = ctx.executable._metadata,
    )

    # Create the archive.
    package = ctx.actions.declare_file("{}.package.tar".format(ctx.label.name))
    ctx.actions.run(
        inputs = depset([manifest], transitive = inputs),
        outputs = [package],
        progress_message = "Generating package for {}".format(ctx.label),
        arguments = ["--output", package.path, manifest.path],
        executable = ctx.executable._package,
    )

    # In addition create an executable rule to manipulate the package
    ctx.actions.write(
        output = ctx.outputs.executable,
        content = """#!/bin/bash
        mkdir -p "$1"
        tar -xf "{}" -C "$1"
        """.format(package.short_path),
        is_executable = True,
    )

    return [
        DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = ctx.runfiles(files = [package]),
            files = depset([package]),
        ),
        OutputGroupInfo(metadata = [metadata]),
    ]

bzd_package = rule(
    implementation = _bzd_package_impl,
    attrs = {
        "deps": attr.label_keyed_string_dict(
            providers = [BzdPackageFragmentInfo],
            doc = "Target or files dependencies to be added to the package.",
        ),
        "include_metadata": attr.bool(
            default = False,
            doc = "Include the metadata as part of the package.",
        ),
        "_buildstamp": attr.label(
            default = Label("@bzd_package//buildstamp:to_json"),
            cfg = "exec",
            executable = True,
        ),
        "_metadata": attr.label(
            default = Label("@bzd_package//metadata"),
            cfg = "exec",
            executable = True,
        ),
        "_package": attr.label(
            default = Label("@bzd_package//:package"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)
