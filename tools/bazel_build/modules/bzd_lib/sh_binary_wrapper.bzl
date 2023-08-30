"""Bash binary rules."""

def _update_runfiles(ctx, runfiles, file):
    """Update the current runfiles and return the executable."""

    if type(file) == "File":
        runfiles = runfiles.merge_all([ctx.runfiles(
            files = [file],
        )])
        return runfiles, file

    # Combine the runfiles and the executable (which is not always captured in the runfiles, especially with files).
    runfiles = runfiles.merge_all([ctx.runfiles(
        files = [file.files_to_run.executable],
    ), file.default_runfiles])

    return runfiles, file.files_to_run.executable

def _file_to_path(ctx, file):
    """Return the path of the file."""

    runfiles_relative_tool_path = ctx.workspace_name + "/" + file.short_path
    return "$RUNFILES_DIR/{}".format(runfiles_relative_tool_path)

def sh_binary_wrapper_impl(ctx, output, binary = None, locations = {}, data = [], extra_runfiles = [], command = "{binary} $@"):
    """Bash binary wrapper rule.

    Args:
        ctx: The rule context.
        output: The output file to be generated.
        binary: The binary file.
        locations: dictionary of locations to be expanded.
        data: Additional data to be added.
        extra_runfiles: Additional runfiles to be added.
        command: The command to be used.

    Returns:
        A DefaultInfo provider pointing to the new binary rule.
    """

    # Prepare the runfiles for the execution
    runfiles = ctx.runfiles(
        files = data,
    ).merge_all(extra_runfiles)

    # Add binaries
    extra_substitutions = {}

    locations_to_key = dict(locations)
    locations_to_key.update({binary: "binary"} if binary else {})

    for location, key in locations_to_key.items():
        runfiles, extra_file = _update_runfiles(ctx, runfiles, location)
        extra_substitutions[key] = _file_to_path(ctx, extra_file)

    command_pre = """#!/bin/bash
    set -e
    if [ -z "$RUNFILES_DIR" ]; then
        export RUNFILES_DIR="$0.runfiles"
    fi
    """

    # Create the wrapping script
    ctx.actions.write(
        output = output,
        is_executable = True,
        content = command_pre + command.format(
            root = "$RUNFILES_DIR",
            workspace = "$RUNFILES_DIR/{}".format(ctx.workspace_name),
            **extra_substitutions
        ),
    )

    return DefaultInfo(
        executable = output,
        runfiles = runfiles,
    )

def _sh_binary_wrapper_impl(ctx):
    return sh_binary_wrapper_impl(
        ctx = ctx,
        binary = ctx.attr.binary,
        locations = ctx.attr.locations,
        output = ctx.outputs.executable,
        data = ctx.files.data,
        command = ctx.attr.command,
    )

sh_binary_wrapper = rule(
    doc = "Binary wrapper rule. Wraps a bazel executable into this rule and decorates it with custom arguments or wrapping script.",
    implementation = _sh_binary_wrapper_impl,
    attrs = {
        "binary": attr.label(
            allow_files = True,
            executable = True,
            cfg = "exec",
            doc = "Label or file of the binary to be wrapped.",
        ),
        "command": attr.string(
            default = "{binary} $@",
            doc = "Content of the wrapping script, by default it simply forwards all command line arguments to the binary.",
        ),
        "data": attr.label_list(
            allow_files = True,
        ),
        "locations": attr.label_keyed_string_dict(
            allow_files = True,
            cfg = "exec",
            doc = "Executables or files to add to the runfiles and that can be accessed via their associated string.",
        ),
    },
    executable = True,
)
