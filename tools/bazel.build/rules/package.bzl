_BzdPackageFragment = provider(fields = ["root", "files", "files_remap", "tars"])

"""
Generate a package provider
"""
def bzd_package_fragment(ctx, **kwargs):
    return _BzdPackageFragment(
        root = "{}.{}".format(ctx.label.package.replace("/", "."), ctx.label.name),
        **kwargs
    )

def _bzd_package_impl(ctx):

    package = ctx.actions.declare_file("package.tar")
    package_creation_commands = [
        "tar -h -cf \"{}\" -T /dev/null".format(package.path)
    ]

    tar_cmd = "tar -h --hard-dereference -f \"{}\"".format(package.path)
    inputs = []
    for target in ctx.attr.deps:
        if _BzdPackageFragment in target:
            fragment = target[_BzdPackageFragment]

            root = target[_BzdPackageFragment].root
            if not root:
                fail("Value 'root' for a package fragment cannot be empty.")

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
                        "{} --concatenate \"temp.tar\"".format(tar_cmd)
                    ]

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
