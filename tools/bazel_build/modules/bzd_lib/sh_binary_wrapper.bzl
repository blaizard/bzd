"""Bash binary rules.

This uses the runfiles structured as described here: https://github.com/bazelbuild/examples/blob/main/rules/runfiles/tool.bzl
"""

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

def _expand_path(ctx, path):
    """Return the expanded path from the rule context."""

    runfiles_relative_tool_path = ctx.workspace_name + "/" + path
    return "$RUNFILES_DIR/{}".format(runfiles_relative_tool_path)

def sh_binary_wrapper_impl(ctx, output, binary = None, locations = {}, paths = {}, data = [], extra_runfiles = [], command = "{binary} $@", root_symlinks = None, symlinks = None):
    """Bash binary wrapper rule.

    Args:
        ctx: The rule context.
        output: The output file to be generated.
        binary: The binary file.
        locations: dictionary of locations to be expanded.
        paths: dictionary of paths to be expanded.
        data: Additional data to be added.
        extra_runfiles: Additional runfiles to be added.
        command: The command to be used.
        root_symlinks: A dictionary mapping paths to files, where the paths are relative to the root
                       of the runfiles directory.
        symlinks: A dictionary mapping paths to files, where paths are implicitly prefixed with the
                  name of the main workspace.

    Returns:
        A DefaultInfo provider pointing to the new binary rule.
    """

    # Prepare the runfiles for the execution
    runfiles = ctx.runfiles(
        files = data,
        root_symlinks = root_symlinks or {},
        symlinks = symlinks or {},
    ).merge_all(extra_runfiles)

    # Add binaries
    extra_substitutions = {}

    locations_to_key = dict(locations)
    locations_to_key.update({binary: "binary"} if binary else {})

    # Expand locations
    for location, key in locations_to_key.items():
        runfiles, extra_file = _update_runfiles(ctx, runfiles, location)
        extra_substitutions[key] = _expand_path(ctx, extra_file.short_path)

    # Expand paths
    for path, key in paths.items():
        extra_substitutions[key] = _expand_path(ctx, path)

    command_pre = """#!/usr/bin/env bash
    set -o pipefail -o errexit

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
        # Important so that any rules that depends on this one, will get all the files.
        files = runfiles.files,
    )

def _sh_binary_wrapper_impl(ctx):
    return sh_binary_wrapper_impl(
        ctx = ctx,
        binary = ctx.attr.binary,
        locations = ctx.attr.locations,
        paths = ctx.attr.paths,
        output = ctx.outputs.executable,
        data = ctx.files.data,
        command = ctx.attr.command,
        symlinks = {v: k.files.to_list()[0] for k, v in ctx.attr.symlinks.items()},
        root_symlinks = {v: k.files.to_list()[0] for k, v in ctx.attr.root_symlinks.items()},
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
        "paths": attr.string_dict(
            doc = "Short paths that can be accessed via their associated string.",
        ),
        "root_symlinks": attr.label_keyed_string_dict(
            allow_files = True,
            cfg = "exec",
            doc = "A dictionary mapping paths to files, where the paths are relative to the root of the runfiles directory.",
        ),
        "symlinks": attr.label_keyed_string_dict(
            allow_files = True,
            cfg = "exec",
            doc = "A dictionary mapping paths to files, where paths are implicitly prefixed with the name of the main workspace.",
        ),
    },
    executable = True,
)
