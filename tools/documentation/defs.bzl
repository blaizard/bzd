_script_template = """
#!/bin/bash

python3 "{script}" --doxygen {doxygen} $@
"""

def _impl(ctx):

    # Generate the doxyfile
    doxyfile = ctx.actions.declare_file(ctx.label.name + ".doxyfile")
    doxygen_output = ctx.actions.declare_directory(ctx.label.name + ".doxygen")

    ctx.actions.expand_template(
        template = ctx.file._doxyfile_template,
        output = doxyfile,
        substitutions = {
            "{input}": " \\\n".join(['"{}"'.format(t.path) for t in ctx.files.srcs]),
            "{output}": doxygen_output.path
        },
    )
    print("Idenfitied {} source(s) files".format(len(ctx.files.srcs)))

    # Generate the XML output of the doxygen
    args = ctx.actions.args()
    args.add(doxyfile.path)

    ctx.actions.run(
        outputs = [ doxygen_output ],
        inputs = ctx.files.srcs + [ doxyfile ],
        arguments = [ args ],
        progress_message = "Generating doxygen XML into '%s'" % doxyfile.path,
        executable = ctx.executable._doxygen,
    )

    # Generate the documentation from the XML output
    script = ctx.actions.declare_file("documentation-%s-generator" % ctx.label.name)
    script_content = _script_template.format(
        script = "tools/documentation/documentation.py", #ctx.executable._generator.short_path,
        doxygen = doxygen_output.short_path,
    )
    ctx.actions.write(script, script_content, is_executable = True)

    runfiles = ctx.runfiles(files = [doxygen_output], collect_data = True)

    return [DefaultInfo(executable = script, runfiles = runfiles)]

cc_documentation = rule(
    implementation = _impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Input C++ source/header files.",
        ),
        "data": attr.label_list(
            allow_files = True,
            doc = "Data files available to binaries using this library",
        ),
        "_generator": attr.label(
            default = Label("//tools/documentation:generator"),
            allow_files = True,
            cfg = "host",
            executable = True,
        ),
        "_doxygen": attr.label(
            default = Label("@doxygen//:bin"),
            allow_files = True,
            cfg = "host",
            executable = True,
        ),
        "_doxyfile_template": attr.label(
            default = Label("//tools/documentation:Doxyfile"),
            allow_single_file = True,
        ),
    },
    executable = True
)
