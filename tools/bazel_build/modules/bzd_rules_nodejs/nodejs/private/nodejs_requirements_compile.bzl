"""Requirements compilation."""

load("@rules_python//python:defs.bzl", "py_binary")

def bzd_nodejs_requirements_compile(name, srcs, output):
    """Compile the requirements files into a json format including dependencies."""

    compile_py = Label("//nodejs/private/requirements:compile.py")
    npm = Label("@node//:npm")

    py_binary(
        name = name,
        main = compile_py,
        srcs = [
            compile_py,
        ],
        args = [
            "--output",
            "$(rlocationpath {})".format(output),
            "--npm",
            "$(rootpath {})".format(npm),
        ] + ["$(rootpath {})".format(src) for src in srcs],
        data = srcs + [output, npm],
        deps = [
            "@bzd_python//bzd/utils:run",
            "@bzd_python//bzd/utils:semver",
        ],
    )
