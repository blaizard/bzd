def _sh_binary_wrapper_impl(ctx):
    binary = ctx.attr.binary
    executable = binary.files_to_run.executable

    # Prepare the runfiles for the execution
    runfiles = ctx.runfiles(
		files = [executable] + ctx.files.data
	)
    runfiles = runfiles.merge(binary.default_runfiles)

    runfiles_relative_tool_path = ctx.workspace_name + "/" + executable.short_path
    command_pre = "#!/bin/bash\nexport RUNFILES_DIR=\"$0.runfiles\"\n"
    binary_path = "${{RUNFILES_DIR}}/{}".format(runfiles_relative_tool_path)
    # Create the wrapping script
    command_format = ctx.attr.command
    ctx.actions.write(
        output = ctx.outputs.executable,
        is_executable = True,
		content = command_pre + command_format.format(
            binary = binary_path,
			# to be fixed
            binary2 =  executable.short_path,
        ),
    )

    return DefaultInfo(
        executable = ctx.outputs.executable,
        runfiles = runfiles,
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
		)
    },
    executable = True,
)
