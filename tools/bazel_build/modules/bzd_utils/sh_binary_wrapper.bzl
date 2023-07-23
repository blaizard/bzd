def _update_runfiles(ctx, runfiles, binary):
    """Update the current runfiles and return the executable."""

    # Combine the runfiles and the executable (which is not always captured in the runfiles, especially with files).
    runfiles = runfiles.merge_all([ctx.runfiles(
        files = [binary.files_to_run.executable],
    ), binary.default_runfiles])

    return runfiles, binary.files_to_run.executable

def _executable_to_path(ctx, executable):
    """Return the path of the executable."""

    runfiles_relative_tool_path = ctx.workspace_name + "/" + executable.short_path
    return "$RUNFILES_DIR/{}".format(runfiles_relative_tool_path)

def sh_binary_wrapper_impl(ctx, output, binary = None, locations = {}, extra_runfiles = [], expand_targets = [], symlinks = {}, root_symlinks = {}, files = None, command = "{binary} $@"):
    # Prepare the runfiles for the execution
    runfiles = ctx.runfiles(
        files = extra_runfiles,
        symlinks = symlinks,
        root_symlinks = root_symlinks,
    )

    # Add binaries
    extra_substitutions = {}

    locations_to_key = dict(locations)
    locations_to_key.update({binary: "binary"} if binary else {})

    for location, key in locations_to_key.items():
        runfiles, extra_executable = _update_runfiles(ctx, runfiles, location)
        extra_substitutions[key] = _executable_to_path(ctx, extra_executable)

    command_pre = """#!/bin/bash
    set -e
    if [ -z "$RUNFILES_DIR" ]; then
        export RUNFILES_DIR="$0.runfiles"
    fi
    """

    # Expand the location targets
    command_expanded = ctx.expand_location(command, targets = expand_targets)

    # Create the wrapping script
    ctx.actions.write(
        output = output,
        is_executable = True,
        content = command_pre + command_expanded.format(
            root = "$RUNFILES_DIR",
            workspace = "$RUNFILES_DIR/{}".format(ctx.workspace_name),
            **extra_substitutions
        ),
    )

    return DefaultInfo(
        executable = output,
        runfiles = runfiles,
        files = files,
    )

def _sh_binary_wrapper_impl(ctx):
    return sh_binary_wrapper_impl(
        ctx = ctx,
        binary = ctx.attr.binary,
        locations = ctx.attr.locations,
        output = ctx.outputs.executable,
        extra_runfiles = ctx.files.data,
        expand_targets = ctx.attr.data,
        symlinks = ctx.attr.symlinks,
        root_symlinks = ctx.attr.root_symlinks,
        command = ctx.attr.command,
    )

"""Binary wrapper rule. Wraps a bazel executable into this rule
and decorates it with custom arguments or wrapping script.
"""
sh_binary_wrapper = rule(
    implementation = _sh_binary_wrapper_impl,
    attrs = {
        "binary": attr.label(
            allow_files = True,
            executable = True,
            cfg = "exec",
            doc = "Label or file of the binary to be wrapped.",
        ),
        "locations": attr.label_keyed_string_dict(
            allow_files = True,
            cfg = "exec",
            doc = "Executables or files to add to the runfiles and that can be accessed via their associated string.",
        ),
        "command": attr.string(
            default = "{binary} $@",
            doc = "Content of the wrapping script, by default it simply forwards all command line arguments to the binary.",
        ),
        "data": attr.label_list(
            allow_files = True,
        ),
        "symlinks": attr.label_keyed_string_dict(),
        "root_symlinks": attr.label_keyed_string_dict(),
    },
    executable = True,
)
