"""Rules for tscircuit."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//private:provider.bzl", "BzdTscircuitInfo")

def _file_to_path(file):
    """Path of a file within the project directory."""

    return file.short_path.replace("../", "external/")

def _bzd_tscircuit_project_impl(ctx):
    sources = ctx.files.srcs + [ctx.file.main]
    for dep in ctx.attr.deps:
        sources.extend(dep[BzdTscircuitInfo].files.to_list())

    # Only the sources are exposed, in a dedicated directory of the runfiles, so that
    # the tscircuit CLI does not try to serve the whole runfiles tree.
    root_symlinks = {
        "project/" + _file_to_path(file): file
        for file in sources
    }

    return [DefaultInfo(runfiles = ctx.runfiles(root_symlinks = root_symlinks))]

_bzd_tscircuit_project = rule(
    doc = """Collect the sources of a tscircuit project in a dedicated runfiles directory.""",
    implementation = _bzd_tscircuit_project_impl,
    attrs = {
        "deps": attr.label_list(
            doc = "Libraries this project depends on.",
            providers = [BzdTscircuitInfo],
        ),
        "main": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "The tscircuit file to be evaluated.",
        ),
        "srcs": attr.label_list(
            allow_files = True,
            doc = "Source files of the circuit.",
        ),
    },
)

def _bzd_tscircuit_command(mode, main):
    if mode == "dev":
        return "cd {{root}}/project && {{binary}} dev {main} \"$@\"".format(main = main)
    if mode == "svg":
        return "{{binary}} export {{root}}/project/{main} --format schematic-svg --output \"$@\"".format(main = main)
    if mode == "json":
        return "{{binary}} export {{root}}/project/{main} --format circuit-json --output \"$@\"".format(main = main)
    fail("Unknown mode '{}'.".format(mode))

def _bzd_tscircuit_wrapper_impl(ctx):
    main = _file_to_path(ctx.file.main)

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        binary = ctx.attr._tsci,
        command = _bzd_tscircuit_command(ctx.attr.mode, main),
        output = ctx.outputs.executable,
        extra_runfiles = [
            ctx.attr.project[DefaultInfo].default_runfiles,
        ],
    )]

_bzd_tscircuit_wrapper = rule(
    doc = """Run the tscircuit CLI on a project.""",
    implementation = _bzd_tscircuit_wrapper_impl,
    attrs = {
        "main": attr.label(
            mandatory = True,
            allow_single_file = True,
            doc = "The tscircuit file to be evaluated.",
        ),
        "mode": attr.string(
            mandatory = True,
            values = ["dev", "svg", "json"],
            doc = "The CLI command to run.",
        ),
        "project": attr.label(
            mandatory = True,
            doc = "The sources of the project.",
        ),
        "_tsci": attr.label(
            default = Label("//private:tsci"),
            executable = True,
            cfg = "exec",
            doc = "The tscircuit CLI.",
        ),
    },
    executable = True,
)

def bzd_tscircuit_binary(name, srcs, main, deps = [], visibility = None, tags = None):
    """Run a tscircuit project.

    The target `name` starts the development server, `name.svg` and `name.json`
    generate the schematic and the circuit JSON, the output path is the first
    argument.

    Args:
        name: The name of the target.
        srcs: The source files of the circuit, `main` must be part of it.
        main: The tscircuit file to be evaluated.
        deps: The tscircuit libraries this project depends on.
        visibility: The visibility of the target.
        tags: Additional tags for the target.
    """

    project = name + ".project"

    _bzd_tscircuit_project(
        name = project,
        srcs = srcs,
        main = main,
        deps = deps,
        tags = ["tscircuit", "manual"],
    )

    for suffix, mode in (("", "dev"), (".svg", "svg"), (".json", "json")):
        _bzd_tscircuit_wrapper(
            name = name + suffix,
            project = project,
            main = main,
            mode = mode,
            visibility = visibility,
            tags = ["tscircuit"] + (tags or []),
        )
