"""Generate a bundle for executables."""

load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

def _bzd_bundle_bootstrap_impl(ctx):
    runfiles = ctx.runfiles()
    outputs = []
    for name, executable in ctx.attr.executables.items():
        if "/" in name or "\\" in name:
            fail("The executable file name '{}' cannot contain (back-)slashes.".format(name))
        output = ctx.actions.declare_file("{}.links/{}".format(ctx.label.name, name))
        ctx.actions.expand_template(
            output = output,
            template = ctx.file.template,
            substitutions = {
                "{executable}": executable.files_to_run.executable.short_path,
                "{runfiles_directory}": ctx.label.name + ".runfiles",
                "{workspace_name}": executable.label.workspace_name or ctx.workspace_name,
            },
            is_executable = True,
        )
        outputs.append(output)
        runfiles = runfiles.merge(executable.default_runfiles)

    ctx.actions.write(
        output = ctx.outputs.executable,
        content = "",
        is_executable = True,
    )

    return [
        DefaultInfo(
            executable = ctx.outputs.executable,
            runfiles = runfiles,
            files = depset(outputs),
        ),
    ]

_bzd_bundle_bootstrap = rule(
    doc = "Add a bootstrap scripts to run the given executable.",
    implementation = _bzd_bundle_bootstrap_impl,
    attrs = {
        "executables": attr.string_keyed_label_dict(
            doc = "Bootstrap script path targeting the given executable.",
            cfg = "target",
        ),
        "template": attr.label(
            allow_single_file = True,
            doc = "Template script to used for the bootstrap.",
        ),
    },
    executable = True,
)

def _bzd_bundle_tar_impl(name, visibility, executables, compression, compression_level, output, **kwargs):
    bootstrap_name = "{}.bootstrap".format(name)
    _bzd_bundle_bootstrap(
        name = bootstrap_name,
        executables = executables,
        template = select(
            {
                "@platforms//os:linux": Label("//private/templates:bootstrap.sh.template"),
            },
            no_match_error = "The target platform is not supported.",
        ),
    )
    pkg_tar(
        name = name,
        srcs = [
            bootstrap_name,
        ],
        include_runfiles = True,
        out = output or name,
        extension = compression,
        compression_level = compression_level,
        visibility = visibility,
        **kwargs
    )

bzd_bundle_tar = macro(
    doc = "Create a bundle with bootstrap scripts for the given executables.",
    implementation = _bzd_bundle_tar_impl,
    attrs = {
        "compression": attr.string(
            values = ["gz", "bz2", "xz"],
            doc = "The type of compression for the resulting tarball.",
            configurable = False,
        ),
        "compression_level": attr.int(
            doc = "The compression level for the resulting tarball.",
            default = 6,
            configurable = False,
        ),
        "executables": attr.string_keyed_label_dict(
            doc = "Bootstrap script path targeting the given executable.",
            cfg = "target",
        ),
        "output": attr.output(
            doc = "The name of the output tar file, if unset, it will have the same name as the rule.",
        ),
    },
)
