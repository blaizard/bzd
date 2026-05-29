"""Generate a bundle for executables."""

def _gather_files_and_symlinks(ctx, targets):
    """Gather all files/directories and symlinks for the give target set."""

    files = []
    transitive_files = []
    symlinks = {}

    for src in targets:
        info = src[DefaultInfo]

        # Include the files associated with the target.
        transitive_files.append(info.files)

        # If runfiles are requested and this is an executable.
        if ctx.attr.include_runfiles and info.files_to_run and info.files_to_run.executable:
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

    return depset(files, transitive = transitive_files), symlinks

def _gather_root_symlinks(ctx):
    """Gather all root symlinks."""

    files = []
    symlinks = {}
    for name, target in ctx.attr.root_symlinks.items():
        info = target[DefaultInfo]

        # If this is an executable.
        if ctx.attr.include_runfiles and info.files_to_run and info.files_to_run.executable:
            alias = ctx.actions.declare_file("{}.root_symlinks/{}".format(ctx.label.name, name))
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
            fail("Root symlink '{}' targets {} file(s); it must point to a single file/directory or an executable (if include_runfiles=True).".format(name, len(info.files.to_list())))

    return depset(files), symlinks

def _get_output(ctx, layer_id):
    """Get the output file for the given layer."""

    output = ctx.outputs.output
    if layer_id == 0:
        return output
    parts = output.basename.rsplit(".", 1)
    return ctx.actions.declare_file("{}.{}{}".format(parts[0], layer_id, ("." + parts[1]) if len(parts) > 1 else ""), sibling = output)

def _bzd_tar_impl(ctx):
    layer_id = 0
    layers = []
    transitive_files = []
    outputs = []

    for layer in ctx.attr.layers:
        files, symlinks = _gather_files_and_symlinks(ctx, [layer])
        output = _get_output(ctx, layer_id)
        layers.append({
            "files": sorted([f.path for f in files.to_list()]),
            "output": output.basename,
            "symlinks": symlinks,
        })
        outputs.append(output)
        transitive_files.append(files)
        layer_id += 1

    files, symlinks = _gather_files_and_symlinks(ctx, ctx.attr.srcs)
    root_symlinks_files, root_symlinks = _gather_root_symlinks(ctx)
    output = _get_output(ctx, layer_id)
    layers.append({
        "files": sorted([f.path for f in files.to_list() + root_symlinks_files.to_list()]),
        "output": output.basename,
        "symlinks": symlinks | root_symlinks,
    })
    outputs.append(output)
    transitive_files.extend([files, root_symlinks_files])

    manifest_file = ctx.actions.declare_file("{}.manifest.json".format(ctx.label.name))
    ctx.actions.write(
        output = manifest_file,
        content = json.encode({
            "layers": layers,
        }),
    )

    args = ctx.actions.args()
    args.add("--output", ctx.outputs.output.dirname)
    args.add("--tar", ctx.file._tar)
    if ctx.attr.compression:
        args.add("--compression", ctx.attr.compression)
    args.add(manifest_file)

    ctx.actions.run(
        inputs = depset([manifest_file, ctx.file._tar], transitive = transitive_files),
        outputs = outputs,
        progress_message = "Creating {} tarballs for {}".format(len(layers), str(ctx.label)) if len(layers) > 1 else "Creating tarball for {}".format(str(ctx.label)),
        arguments = [args],
        executable = ctx.executable._bundler,
    )

    return [DefaultInfo(files = depset(outputs))]

_bzd_tar = rule(
    doc = "Package targets into a tar archive.",
    implementation = _bzd_tar_impl,
    attrs = {
        "compression": attr.string(
            values = ["gz", "bz2", "xz"],
            doc = "The type of compression for the resulting tarball.",
        ),
        "include_runfiles": attr.bool(
            default = False,
            doc = "Whether to include runfiles for executable targets.",
        ),
        "layers": attr.label_list(
            doc = "Targets to be included in separate layers in the tarball. The output will be split into multiple tar files, one per layer.",
            cfg = "target",
        ),
        "output": attr.output(
            doc = "The name of the output tar file, if unset, it will have the same name as the rule.",
        ),
        "root_symlinks": attr.string_keyed_label_dict(
            doc = "Root symlinks to be created in the tarball, mapping the symlink name to a target. The target must either be an executable (if include_runfiles=True) or a single file/directory.",
            cfg = "target",
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

def _bzd_bundle_tar_impl(name, visibility, output, **kwargs):
    _bzd_tar(
        name = name,
        template = select(
            {
                "@platforms//os:linux": Label("//private/templates:bootstrap.sh.template"),
            },
            no_match_error = "The target platform is not supported.",
        ),
        output = output or name,
        visibility = visibility,
        **kwargs
    )

bzd_bundle_tar = macro(
    doc = "Create a bundle with bootstrap scripts for the given executables.",
    implementation = _bzd_bundle_tar_impl,
    inherit_attrs = _bzd_tar,
    attrs = {
        "output": attr.output(
            doc = "The name of the output tar file, if unset, it will have the same name as the rule.",
        ),
        "template": None,
    },
)
