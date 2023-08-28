"""NodeJs external binary rule."""

load("@bzd_toolchain_nodejs//nodejs:private/nodejs_install.bzl", "bzd_nodejs_make_node_modules")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_nodejs_extern_binary_impl(ctx):
    _package_json, node_modules = bzd_nodejs_make_node_modules(ctx, ctx.attr.packages, base_dir_name = ctx.label.name + ".install")

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["@bzd_toolchain_nodejs//nodejs:toolchain_type"].executable

    # Build the command
    command = "NODE_PATH={{node_modules}} {{node}} {{node_modules}}/{} ".format(ctx.attr.binary) + " ".join(ctx.attr.params) + " $@"

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            toolchain_executable.node: "node",
            node_modules: "node_modules",
        },
        output = ctx.outputs.executable,
        command = command,
        extra_runfiles = [node_modules],
    )]

bzd_nodejs_extern_binary = rule(
    doc = """Run a nodejs binary from an external package.""",
    implementation = _bzd_nodejs_extern_binary_impl,
    attrs = {
        "binary": attr.string(
            mandatory = True,
            doc = "The binary name to be executed.",
        ),
        "packages": attr.string_dict(
            allow_empty = True,
            doc = "Package dependencies",
        ),
        "params": attr.string_list(
            doc = "Arguments as a string.",
        ),
    },
    executable = True,
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
)
