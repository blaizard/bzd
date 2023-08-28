"""Package rules."""

BzdPackageFragmentInfo = provider("Provider for a package fragment.", fields = ["root", "files", "files_remap", "tars"])
BzdPackageMetadataFragmentInfo = provider("Provider for metadata framgnets.", fields = ["manifests"])
_BzdPackageMetadataInfo = provider("Provider for metadata.", fields = ["manifests"])

# ---- Aspect

def _bzd_package_metadata_aspect_impl(target, ctx):
    manifests = []
    if hasattr(ctx.rule.attr, "dep"):
        manifests += ctx.rule.attr.dep[_BzdPackageMetadataInfo].manifests
    if BzdPackageMetadataFragmentInfo in target:
        manifests += target[BzdPackageMetadataFragmentInfo].manifests
    return [_BzdPackageMetadataInfo(manifests = manifests)]

_bzd_package_metadata_aspect = aspect(
    doc = "Aspects to gather data from bzd depedencies.",
    implementation = _bzd_package_metadata_aspect_impl,
    attr_aspects = ["dep"],
)

# ---- Packages

def _bzd_package_impl(ctx):
    package = ctx.actions.declare_file("{}.package.tar".format(ctx.label.name))
    package_creation_commands = [
        "tar -h -cf \"{}\" -T /dev/null".format(package.path),
    ]

    tar_cmd = "tar -h --hard-dereference -f \"{}\"".format(package.path)
    inputs = []
    manifests = []
    metadata_args = []
    for target, root in ctx.attr.deps.items():
        if _BzdPackageMetadataInfo in target:
            manifests += target[_BzdPackageMetadataInfo].manifests
            for manifest in manifests:
                metadata_args += ["--input", root, manifest.path]

        if BzdPackageFragmentInfo in target:
            fragment = target[BzdPackageFragmentInfo]

            if not root:
                fail("Each package fragment must have a valid root.")

            # Add single files or symlinks
            if hasattr(fragment, "files"):
                for f in fragment.files:
                    inputs.append(f)

                    # Convertion to short_path is needed for generated files
                    package_creation_commands.append("{} --append \"{}\" --transform 's,^{},{}/{},'".format(tar_cmd, f.path, f.path, root, f.short_path))

            # Remap single files or symlinks
            if hasattr(fragment, "files_remap"):
                for path, f in fragment.files_remap.items():
                    inputs.append(f)
                    package_creation_commands.append("{} --append \"{}\" --transform 's,^{},{}/{},'".format(tar_cmd, f.path, f.path, root, path))

            # Add tar archive
            if hasattr(fragment, "tars"):
                for f in fragment.tars:
                    inputs.append(f)
                    package_creation_commands += [
                        "mkdir -p temp",
                        "tar -xf \"{}\" -C \"./temp\"".format(f.path),
                        "tar -cf \"temp.tar\" --transform 's,^temp/,{}/,' --remove-files temp/".format(root),
                        "tar -f \"temp.tar\" --delete temp/",
                        "{} --concatenate \"temp.tar\"".format(tar_cmd),
                    ]

        else:
            fail("Dependencies for the rule '{}' requires BzdPackageFragmentInfo provider.".format(target.label))

    # Build buildstamp
    buildstamp = ctx.actions.declare_file("{}.buildstamp.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [],
        outputs = [buildstamp],
        progress_message = "Generating buildstamp for {}".format(ctx.label),
        arguments = [buildstamp.path],
        executable = ctx.attr._buildstamp.files_to_run,
    )

    # Merge all metadata fragments together
    metadata = ctx.actions.declare_file("{}.metadata.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = manifests + [buildstamp],
        outputs = [metadata],
        progress_message = "Generating manifest for {}".format(ctx.label),
        arguments = metadata_args + ["--input", "", buildstamp.path, metadata.path],
        executable = ctx.attr._metadata.files_to_run,
    )

    # If the metadata is request, add it to the package
    if ctx.attr.include_metadata:
        inputs.append(metadata)
        package_creation_commands.append("{} --append \"{}\" --transform 's,^{},metadata.json,'".format(tar_cmd, metadata.path, metadata.path))

    # Build the actual package
    ctx.actions.run_shell(
        inputs = inputs,
        outputs = [package],
        progress_message = "Generating package for {}".format(ctx.label),
        command = "\n".join(package_creation_commands),
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

    runfiles = ctx.runfiles(files = [package])
    return [
        DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = runfiles,
            files = depset([package]),
        ),
        OutputGroupInfo(metadata = [metadata]),
    ]

bzd_package = rule(
    implementation = _bzd_package_impl,
    attrs = {
        "deps": attr.label_keyed_string_dict(
            aspects = [_bzd_package_metadata_aspect],
            doc = "Target or files dependencies to be added to the package.",
        ),
        "include_metadata": attr.bool(
            default = False,
            doc = "Include the metadata as part of the package.",
        ),
        "_buildstamp": attr.label(
            default = Label("@bzd_package//buildstamp:to_json"),
        ),
        "_metadata": attr.label(
            default = Label("@bzd_package//metadata"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)
