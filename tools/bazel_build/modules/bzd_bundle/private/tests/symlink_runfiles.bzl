"""Executable with a symlinked runfile that is dangling outside of the runfiles tree."""

def _symlink_runfiles_impl(ctx):
    executable = ctx.actions.declare_file(ctx.label.name + ".sh")
    ctx.actions.write(
        output = executable,
        content = "#!/usr/bin/env bash\nset -e\ncat \"$(dirname \"$0\")/link.txt\"\n",
        is_executable = True,
    )

    link = ctx.actions.declare_symlink("link.txt")
    ctx.actions.symlink(
        output = link,
        target_path = ctx.file.data.basename,
    )

    return [DefaultInfo(
        executable = executable,
        files = depset([executable]),
        runfiles = ctx.runfiles(files = [
            link,
            ctx.file.data,
        ]),
    )]

symlink_runfiles = rule(
    doc = "Create an executable with a symlinked runfile.",
    implementation = _symlink_runfiles_impl,
    attrs = {
        "data": attr.label(
            allow_single_file = True,
            mandatory = True,
        ),
    },
    executable = True,
)
