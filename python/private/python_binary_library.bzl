"""Expose binaries to a python target."""

def _bzd_python_binary_library_impl(ctx):
    output_py = ctx.actions.declare_file(ctx.label.name + ".py")

    content = ["# This file was auto generated."]
    runfiles = ctx.runfiles(files = [output_py])
    for executable, name in ctx.attr.executables.items():
        info = executable[DefaultInfo]
        content.append("{name} = \"{path}\"".format(
            name = name,
            path = info.files_to_run.executable.short_path,
        ))
        runfiles = runfiles.merge_all([ctx.runfiles(
            files = [info.files_to_run.executable],
        ), info.default_runfiles])

    ctx.actions.write(
        output = output_py,
        content = "\n".join(content),
    )

    return [
        PyInfo(
            transitive_sources = depset([output_py]),
        ),
        DefaultInfo(
            runfiles = runfiles,
        ),
    ]

bzd_python_binary_library = rule(
    implementation = _bzd_python_binary_library_impl,
    doc = "Expose binaries to a python target.",
    attrs = {
        "executables": attr.label_keyed_string_dict(
            doc = "Executables to expose to a python target, value is the variable.",
            cfg = "target",
        ),
    },
    provides = [DefaultInfo, PyInfo],
)
