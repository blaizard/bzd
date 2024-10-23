"""Requirements compilation."""

load("@bzd_rules_nodejs_pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

def bzd_nodejs_requirements_compile(name, srcs, output):
    """Compile the requirements files into a json format including dependencies."""

    compile_py = Label("//nodejs/private/requirements:compile.py")
    pnpm = Label("//nodejs:pnpm")

    py_binary(
        name = name,
        main = compile_py,
        srcs = [
            compile_py,
        ],
        args = [
            "--output",
            "$(rlocationpath {})".format(output),
            "--pnpm",
            "$(rootpath {})".format(pnpm),
        ] + ["$(rootpath {})".format(src) for src in srcs],
        data = srcs + [output, pnpm],
        deps = [
            "@bzd_python//bzd/utils:run",
            requirement("pyyaml"),
        ],
    )
