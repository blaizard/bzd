"""Requirements compilation."""

load("@rules_python//python:defs.bzl", "py_binary")

def _bzd_nodejs_requirements_compile_impl(name, srcs, output, output_path, **kwargs):
    compile_py = Label("//nodejs/requirements:compile.py")
    npm = Label("@node//:npm")
    data = srcs + [npm] + ([output] if output else [])

    py_binary(
        name = name,
        main = compile_py,
        srcs = [
            compile_py,
        ],
        args = ([
            "--output",
            "$(rlocationpath {})".format(output),
        ] if output else []) + (["--output-path", output_path] if output_path else []) + [
            "--npm",
            "$(rootpath {})".format(npm),
        ] + ["$(rootpath {})".format(src) for src in srcs],
        data = data,
        deps = [
            "@bzd_python//bzd/utils:run",
            "@bzd_python//bzd/utils:semver",
        ],
        **kwargs
    )

bzd_nodejs_requirements_compile = macro(
    doc = "Compile the requirements files into a json format including dependencies.",
    attrs = {
        "args": None,
        "data": None,
        "deps": None,
        "main": None,
        "output": attr.label(doc = "Location of the processed output.", configurable = False),
        "output_path": attr.string(doc = "Location of the processed output as a string.", configurable = False),
        "srcs": attr.label_list(mandatory = True, doc = "The requirements to be processed.", configurable = False),
    },
    implementation = _bzd_nodejs_requirements_compile_impl,
)
