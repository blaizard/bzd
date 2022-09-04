def _multi_run_impl(ctx):
    runfiles = ctx.runfiles(files = [])
    executables = []

    for target in ctx.attr.targets:
        executables.append(target.files_to_run.executable)
        runfiles = runfiles.merge(target.default_runfiles)

    ctx.actions.write(
        output = ctx.outputs.executable,
        content = """#!/bin/bash
        set -e
        if [ -z "$RUNFILES_DIR" ]; then
            export RUNFILES_DIR="$0.runfiles"
        fi
        {}
        """.format("\n".join(["\"$RUNFILES_DIR/{}\"".format(ctx.workspace_name + "/" + executable.short_path) for executable in executables])),
        is_executable = True,
    )

    return [DefaultInfo(executable = ctx.outputs.executable, runfiles = runfiles)]

multi_run = rule(
    doc = "Execute target sequentially from a single bazel run command",
    implementation = _multi_run_impl,
    attrs = {
        "targets": attr.label_list(
            cfg = "target",
            mandatory = True,
            doc = "List of targets to run.",
        ),
    },
    executable = True,
)
