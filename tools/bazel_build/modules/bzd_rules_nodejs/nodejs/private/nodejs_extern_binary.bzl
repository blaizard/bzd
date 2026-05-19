"""NodeJs external binary rule."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//nodejs:private/nodejs_install.bzl", "bzd_nodejs_add_symlink_runfiles", "bzd_nodejs_make_node_modules")
load("//nodejs:private/nodejs_package.bzl", "BzdNodeJsPackageInfo")

def _bzd_nodejs_extern_binary_impl(ctx):
    result = bzd_nodejs_make_node_modules(ctx, ctx.attr.packages, base_dir_name = ctx.label.name + ".install")

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//nodejs:toolchain_type"].executable

    # Build the command
    command = """
export NODE_ENV=production
export NODE_MODULES="$(dirname {{package_json}})/node_modules"

{{node}} $NODE_MODULES/{binary} {params} "$@"
""".format(
        binary = ctx.attr.binary,
        params = " ".join(ctx.attr.params),
    )

    # Build runfiles with extraction outputs and declare_symlink artifacts.
    # The declare_symlink artifacts are passed via `data` (ctx.runfiles(files=...))
    # rather than symlinks to avoid a Bazel 9.1.0 NullPointerException.
    default_info = sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            toolchain_executable.node: "node",
            result.package_json: "package_json",
        },
        output = ctx.outputs.executable,
        command = command,
        data = result.source_dirs.values() + result.symlinks,
        symlinks = None,
    )

    # Add symlink target entries to the runfiles with the install prefix.
    # The symlink_targets keys (e.g., "node_modules/mocha") are relative to
    # the install directory (base_dir_name), but runfiles need paths relative
    # to the rule's package directory.
    runfiles = bzd_nodejs_add_symlink_runfiles(
        ctx,
        default_info.default_runfiles,
        result.symlink_targets,
        prefix = ctx.label.package + "/" + ctx.label.name + ".install",
    )

    return [DefaultInfo(
        executable = default_info.files_to_run.executable if default_info.files_to_run else None,
        runfiles = runfiles,
        files = default_info.files,
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
        "_extract": attr.label(
            default = "//nodejs/private/python:extract",
            doc = "The extract binary.",
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
    toolchains = ["//nodejs:toolchain_type"],
)
