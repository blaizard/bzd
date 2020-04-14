def _impl(ctx):
    ctx.actions.write(
        output = ctx.outputs.executable,
        content = """#!/bin/bash
        set -e
        cp {} "$1"
        """.format(" ".join(["\"{}\"".format(f.short_path) for f in ctx.files.inputs])),
        is_executable = True,
    )

    runfiles = ctx.runfiles(files = ctx.files.inputs)
    return [DefaultInfo(executable = ctx.outputs.executable, runfiles = runfiles)]

copy_outputs = rule(
    implementation = _impl,
    attrs = {
        "inputs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Input labels or files that will .",
        ),
    },
    executable = True,
)
