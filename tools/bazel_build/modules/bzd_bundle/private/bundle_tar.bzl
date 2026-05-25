"""Generate a bundle for executables."""

def _bzd_tar_impl(ctx):
    # Build the files/directories/links to be included in the tarball.
    files = []
    transitive_files = []
    symlinks = {}
    for src in ctx.attr.srcs:
        info = src[DefaultInfo]

        # Include the files associated with the target.
        transitive_files.append(info.files)

        # If this is an executable.
        if info.files_to_run and info.files_to_run.executable:
            base_path = "{}.runfiles/{}".format(info.files_to_run.executable.path, src.label.workspace_name or ctx.workspace_name)

            # Add the executable file itself.
            files.append(info.files_to_run.executable)
            symlinks["{}/{}".format(base_path, info.files_to_run.executable.short_path)] = {"target": info.files_to_run.executable.path, "type": "absolute"}

            # Add associated runfiles.
            runfiles = info.default_runfiles
            if runfiles:
                for f in runfiles.files.to_list():
                    symlinks["{}/{}".format(base_path, f.short_path)] = {"target": f.path, "type": "absolute"}
                for symlink in runfiles.symlinks.to_list():
                    if symlink.target_file:
                        symlinks["{}/{}".format(base_path, symlink.path)] = {"target": symlink.target_file.path, "type": "absolute"}
                    else:
                        symlinks["{}/{}".format(base_path, symlink.path)] = {"target": symlink.target_path, "type": "relative"}
                for symlink in runfiles.root_symlinks.to_list():
                    if symlink.target_file:
                        symlinks["{}/{}".format(base_path, symlink.path)] = {"target": symlink.target_file.path, "type": "absolute"}
                    else:
                        symlinks["{}/{}".format(base_path, symlink.path)] = {"target": symlink.target_path, "type": "relative"}
                transitive_files.append(runfiles.files)

    # Create aliases to targets.
    for name, target in ctx.attr.aliases.items():
        info = target[DefaultInfo]

        # If this is an executable.
        if info.files_to_run and info.files_to_run.executable:
            alias = ctx.actions.declare_file("{}.aliases/{}".format(ctx.label.name, name))
            ctx.actions.expand_template(
                output = alias,
                template = ctx.file.template,
                substitutions = {
                    "{executable}": info.files_to_run.executable.short_path,
                    "{runfiles_directory}": info.files_to_run.executable.path + ".runfiles",
                    "{workspace_name}": target.label.workspace_name or ctx.workspace_name,
                },
                is_executable = True,
            )
            files.append(alias)
            symlinks[name] = {"target": alias.path, "type": "absolute"}

        elif len(info.files.to_list()) == 1:
            symlinks[name] = {"target": info.files.to_list()[0].path, "type": "absolute"}

        else:
            fail("Alias '{}' must point to a single file/directory or an executable.".format(name))

    all_files = depset(files, transitive = transitive_files)

    manifest_file = ctx.actions.declare_file("{}.manifest.json".format(ctx.label.name))
    ctx.actions.write(
        output = manifest_file,
        content = json.encode({
            "files": sorted([f.path for f in all_files.to_list()]),
            "symlinks": symlinks,
        }),
    )

    args = ctx.actions.args()
    args.add("--output", ctx.outputs.output)
    args.add("--tar", ctx.file._tar)
    if ctx.attr.compression:
        args.add("--compression", ctx.attr.compression)
    args.add(manifest_file)

    ctx.actions.run(
        inputs = depset([manifest_file, ctx.file._tar], transitive = [all_files]),
        outputs = [ctx.outputs.output],
        progress_message = "Creating tarball {}".format(str(ctx.label)),
        arguments = [args],
        executable = ctx.executable._bundler,
    )

    return [DefaultInfo(files = depset([ctx.outputs.output]))]

_bzd_tar = rule(
    doc = "Package targets into a tar archive.",
    implementation = _bzd_tar_impl,
    attrs = {
        "aliases": attr.string_keyed_label_dict(
            doc = "Map of alias name to target label. If the target is an executable, an alias script will be created in the tarball that points to the executable.",
            cfg = "target",
        ),
        "compression": attr.string(
            values = ["gz", "bz2", "xz"],
            doc = "The type of compression for the resulting tarball.",
        ),
        "output": attr.output(
            doc = "The name of the output tar file, if unset, it will have the same name as the rule.",
        ),
        "srcs": attr.label_list(
            doc = "Targets to be included in the tarball.",
            cfg = "target",
        ),
        "template": attr.label(
            allow_single_file = True,
            doc = "Template script to used for the bootstrap.",
        ),
        "_bundler": attr.label(
            default = Label("//private/python:bundler"),
            cfg = "exec",
            executable = True,
        ),
        "_tar": attr.label(
            default = Label("@tar//:tar"),
            cfg = "exec",
            executable = True,
            allow_single_file = True,
        ),
    },
)

def _bzd_bundle_tar_impl(name, visibility, executables, output, tags, **kwargs):
    _bzd_tar(
        name = name,
        aliases = executables,
        template = select(
            {
                "@platforms//os:linux": Label("//private/templates:bootstrap.sh.template"),
            },
            no_match_error = "The target platform is not supported.",
        ),
        srcs = executables.values(),
        output = output or name,
        visibility = visibility,
        tags = (tags or []) + ["no-remote-cache"],
        **kwargs
    )

bzd_bundle_tar = macro(
    doc = "Create a bundle with bootstrap scripts for the given executables.",
    implementation = _bzd_bundle_tar_impl,
    inherit_attrs = _bzd_tar,
    attrs = {
        "aliases": None,
        "executables": attr.string_keyed_label_dict(
            doc = "Bootstrap script path targeting the given executable.",
            cfg = "target",
            configurable = False,
        ),
        "output": attr.output(
            doc = "The name of the output tar file, if unset, it will have the same name as the rule.",
        ),
        "srcs": None,
        "template": None,
    },
)
