"""NodeJs external binary rule."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//nodejs:private/nodejs_install.bzl", "bzd_nodejs_make_node_modules")
load("//nodejs:private/nodejs_library.bzl", "BzdNodeJsPackageInfo")

def _bzd_nodejs_extern_binary_impl(ctx):
    _package_json, node_modules = bzd_nodejs_make_node_modules(ctx, ctx.attr.packages, base_dir_name = ctx.label.name + ".install")

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//nodejs:toolchain_type"].executable

    # Build the command
    command = "NODE_ENV=production {{node}} {{node_modules}}/{} ".format(ctx.attr.binary) + " ".join(ctx.attr.params) + " $@"

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            toolchain_executable.node: "node",
            node_modules: "node_modules",
        },
        output = ctx.outputs.executable,
        command = command,
        data = [node_modules],
    )]

bzd_nodejs_extern_binary = rule(
    doc = """Run a nodejs binary from an external package.""",
    implementation = _bzd_nodejs_extern_binary_impl,
    attrs = {
        "binary": attr.string(
            mandatory = True,
            doc = "The binary name to be executed.",
        ),
        "packages": attr.label_list(
            doc = "Package dependencies.",
            providers = [BzdNodeJsPackageInfo],
        ),
        "params": attr.string_list(
            doc = "Arguments as a string.",
        ),
    },
    executable = True,
    toolchains = ["//nodejs:toolchain_type"],
)
