BzdPackageProvider = provider(fields = ["files", "files_remap"])

def _bzd_package_impl(ctx):

    package = ctx.actions.declare_file("package.tar")
    package_creation_commands = [
        "tar -h -cf \"{}\" -T /dev/null".format(package.path)
    ]

    tar_cmd = "tar -h --hard-dereference -rf \"{}\"".format(package.path)
    inputs = []
    for target in ctx.attr.deps:
        if BzdPackageProvider in target:

            # Add single files or symlinks
            for f in target[BzdPackageProvider].files:
                inputs.append(f)
                package_creation_commands.append("{} \"{}\"".format(tar_cmd, f.path))

            # Remap single files or symlinks
            for path, f in target[BzdPackageProvider].files_remap.items():
                inputs.append(f)
                package_creation_commands.append("{} \"{}\" --transform 's,^{},{},'".format(tar_cmd, f.path, f.path, path))
        else:
            fail("Dependencies for this rule requires BzdPackageProvider provider.")

    ctx.actions.run_shell(
        inputs = inputs,
        outputs = [package],
        progress_message = "Generating package for {}".format(ctx.label),
        command = "\n".join(package_creation_commands)
    )

    return [
        DefaultInfo(
            files = depset([package])
        ),
    ]

bzd_package = rule(
    implementation = _bzd_package_impl,
    attrs = {
        "deps": attr.label_list(
            doc = "Target or files dependencies to be added to the package.",
        ),
    },
)
