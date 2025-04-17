"""Create a simple launcher script to use the bazel toolchain instead of the system one for the bootstrapping."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_python_hermetic_launcher_impl(ctx):
    py_runtime = ctx.toolchains["@bazel_tools//tools/python:toolchain_type"].py3_runtime

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        binary = ctx.attr.binary,
        output = ctx.outputs.executable,
        paths = {py_runtime.interpreter.short_path: "py_interpreter"},
        data = depset([py_runtime.interpreter], transitive = [py_runtime.files]).to_list(),
        root_symlinks = {
            "launcher": ctx.outputs.executable,
        },
        command = """
export PATH=$(dirname {py_interpreter}):$PATH
exec {py_interpreter} {binary} "$@"
""",
    )]

bzd_python_hermetic_launcher = rule(
    implementation = _bzd_python_hermetic_launcher_impl,
    doc = "Create a hermetic python launcher, as the one provided by py_binrary relies on the system python, see: https://github.com/bazel-contrib/rules_python/issues/2727.",
    attrs = {
        "binary": attr.label(
            doc = "The python executable to be wrapped.",
            executable = True,
            cfg = "target",
        ),
    },
    toolchains = ["@bazel_tools//tools/python:toolchain_type"],
    provides = [DefaultInfo],
    executable = True,
)
