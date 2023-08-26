load("@bzd_toolchain_nodejs//nodejs:private/utils.bzl", "BzdNodeJsInstallInfo")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

COMMON_EXEC_ATTRS = {
    "executor": attr.label(
        executable = True,
        cfg = "exec",
    ),
    "install": attr.label(
        mandatory = True,
    ),
    "main": attr.label(
        mandatory = True,
        allow_single_file = True,
    ),
    "_allowlist_function_transition": attr.label(
        default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
    ),
    "_metadata_json": attr.label(
        default = Label("@bzd_toolchain_nodejs//nodejs/metadata:metadata_json"),
        allow_single_file = True,
    ),
}

def _bzd_nodejs_transition_impl(settings, attr):
    _ignore = (settings, attr)
    return {"@bzd_toolchain_nodejs//:build_type": "nodejs"}

_bzd_nodejs_transition = transition(
    implementation = _bzd_nodejs_transition_impl,
    inputs = [],
    outputs = ["@bzd_toolchain_nodejs//:build_type"],
)

def _bzd_nodejs_executable_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    # Gather toolchain executable
    executor = ctx.attr.executor if ctx.attr.executor else ctx.toolchains["@bzd_toolchain_nodejs//nodejs:toolchain_type"].executable.node

    # Look for the entry point.
    main = install.transpiled[ctx.file.main.short_path] if ctx.file.main.short_path in install.transpiled else ctx.file.main

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            executor: "binary",
            main: "main",
        },
        output = ctx.outputs.executable,
        command = "BZD_RULE=nodejs {binary} {main} $@",
        symlinks = install.runfiles.symlinks,
    )]

bzd_nodejs_binary = rule(
    doc = "NodeJs binary executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = COMMON_EXEC_ATTRS,
    executable = True,
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
    cfg = _bzd_nodejs_transition,
)

bzd_nodejs_test = rule(
    doc = "NodeJs test executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = COMMON_EXEC_ATTRS,
    executable = True,
    test = True,
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
    cfg = _bzd_nodejs_transition,
)
