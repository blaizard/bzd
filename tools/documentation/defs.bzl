def _impl(ctx):

    # Generate the doxyfile
    doxyfile = ctx.actions.declare_file(ctx.attr.name + ".doxyfile")
    doxygen_output = ctx.actions.declare_directory(ctx.attr.name + ".doxygen")

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
    output = ctx.actions.declare_directory(ctx.attr.name + ".documentation")

    args = ctx.actions.args()
    args.add("--doxygen")
    args.add(doxygen_output.path)
    args.add("--output")
    args.add(output.path)

    ctx.actions.run(
        outputs = [ output ],
        inputs = [ doxygen_output ],
        arguments = [ args ],
        progress_message = "Generating documentation into '%s'" % output.path,
        executable = ctx.executable._generator,
    )

    return [DefaultInfo(files = depset([output]))]

doc_binary = rule(
    implementation = _impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Input C++ source/header files.",
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
    #executable = True
)
