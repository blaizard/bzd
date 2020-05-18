BzdPackageFragment = provider(fields = ["root", "files", "files_remap", "tars"])
BzdPackageMetadataFragment = provider(fields = ["manifests"])

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
        if BzdPackageMetadataFragment in target:
            manifests = target[BzdPackageMetadataFragment].manifests
            for manifest in manifests:
                metadata_args += ["--input", root, manifest.path]

        if BzdPackageFragment in target:
            fragment = target[BzdPackageFragment]

            if not root:
                fail("Each package fragment must have a valid root.")

            # Add single files or symlinks
            if hasattr(fragment, "files"):
                for f in fragment.files:
                    inputs.append(f)
                    package_creation_commands.append("{} --append \"{}\" --transform 's,^,{}/,'".format(tar_cmd, f.path, root))

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
            fail("Dependencies for this rule requires BzdPackageFragment provider.")

    # Merge all metadata fragments together
    metadata = ctx.actions.declare_file("{}.metadata.manifest".format(ctx.label.name))
    ctx.actions.run(
        inputs = manifests,
        outputs = [metadata],
        progress_message = "Generating manifest for {}".format(ctx.label),
        arguments = metadata_args + [metadata.path],
        executable = ctx.attr._metadata.files_to_run,
    )

    ctx.actions.run_shell(
        inputs = inputs,
        outputs = [package],
        progress_message = "Generating package for {}".format(ctx.label),
        command = "\n".join(package_creation_commands),
    )

    # In addition create a executable rule to manipulate the package
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
            doc = "Target or files dependencies to be added to the package.",
        ),
        "_metadata": attr.label(
            default = Label("//tools/bazel.build/rules/assets/package:metadata"),
            cfg = "host",
            executable = True,
        ),
    },
    executable = True,
)
