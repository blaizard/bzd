_BzdPackageFragment = provider(fields = ["root", "files", "files_remap"])

"""
Generate a package provider
"""
def bzd_package_fragment(ctx = None, **kwargs):
    return _BzdPackageFragment(
        root = "{}/{}".format(ctx.label.package, ctx.label.name),
        **kwargs
    )

def _bzd_package_impl(ctx):

    package = ctx.actions.declare_file("package.tar")
    package_creation_commands = [
        "tar -h -cf \"{}\" -T /dev/null".format(package.path)
    ]

    tar_cmd = "tar -h --hard-dereference -rf \"{}\"".format(package.path)
    inputs = []
    for target in ctx.attr.deps:
        if _BzdPackageFragment in target:

            # Add single files or symlinks
            for f in target[_BzdPackageFragment].files:
                inputs.append(f)
                package_creation_commands.append("{} \"{}\"".format(tar_cmd, f.path))

            # Remap single files or symlinks
            for path, f in target[_BzdPackageFragment].files_remap.items():
                inputs.append(f)
                package_creation_commands.append("{} \"{}\" --transform 's,^{},{},'".format(tar_cmd, f.path, f.path, path))
        else:
            fail("Dependencies for this rule requires _BzdPackageFragment provider.")

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
