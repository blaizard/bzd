def sh_binary_wrapper_impl(ctx, binary, output, extra_runfiles = [], symlinks = {}, root_symlinks = {}, files = None, command = "{binary} $@"):
    executable = binary.files_to_run.executable

    # Prepare the runfiles for the execution
    runfiles = ctx.runfiles(
        files = [executable] + extra_runfiles,
        symlinks = symlinks,
        root_symlinks = root_symlinks,
    )
    runfiles = runfiles.merge(binary.default_runfiles)

    runfiles_relative_tool_path = ctx.workspace_name + "/" + executable.short_path
    command_pre = "#!/bin/bash\nif [ -z \"${RUNFILES_DIR}\" ]; then\nexport RUNFILES_DIR=\"$0.runfiles\"\nfi\n"
    binary_path = "${{RUNFILES_DIR}}/{}".format(runfiles_relative_tool_path)

    # Create the wrapping script
    ctx.actions.write(
        output = output,
        is_executable = True,
        content = command_pre + command.format(
            binary = binary_path,
            root = "${{RUNFILES_DIR}}",
            workspace = "${{RUNFILES_DIR}}/{}".format(ctx.workspace_name),
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
        output = ctx.outputs.executable,
        extra_runfiles = ctx.files.data,
        symlinks = ctx.attr.symlinks,
        root_symlinks = ctx.attr.root_symlinks,
        command = ctx.attr.command,
    )

"""
Binary wrapper rule. Wraps a bazel executable into this rule
and decorates it with custom arguments or wrapping script.
"""
sh_binary_wrapper = rule(
    implementation = _sh_binary_wrapper_impl,
    attrs = {
        "binary": attr.label(
            allow_files = True,
            executable = True,
            cfg = "host",
            mandatory = True,
            doc = "Label or file of the binary to be wrapped.",
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
