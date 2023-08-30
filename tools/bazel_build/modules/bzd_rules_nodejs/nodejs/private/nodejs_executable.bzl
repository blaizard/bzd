"""NodeJs executable rules."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_package//:defs.bzl", "BzdPackageFragmentInfo", "bzd_package_prefix_from_file", "bzd_package_to_runfiles")
load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo")

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
    "_coverage_executor": attr.label(
        default = Label("@bzd_rules_nodejs//toolchain/c8"),
        executable = True,
        cfg = "exec",
    ),
    "_metadata_json": attr.label(
        default = Label("@bzd_rules_nodejs//nodejs/metadata:metadata_nodejs.json"),
        allow_single_file = True,
    ),
}

def _bzd_nodejs_transition_impl(_settings, _attr):
    return {"@bzd_rules_nodejs//:build_type": "nodejs"}

# Transition to notify the dependency graph that this is a `nodejs` build.
_bzd_nodejs_transition = transition(
    implementation = _bzd_nodejs_transition_impl,
    inputs = [],
    outputs = ["@bzd_rules_nodejs//:build_type"],
)

def _bzd_nodejs_executable_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    # Create the package provider.
    package = BzdPackageFragmentInfo(
        files = {
            bzd_package_prefix_from_file(install.package_json, depth_from_root = 1): install.files,
        },
    )

    # Look for the entry point.
    main = install.transpiled[ctx.file.main.short_path] if ctx.file.main.short_path in install.transpiled else ctx.file.main

    # Gather toolchain executable.
    executor = ctx.attr.executor if ctx.attr.executor else ctx.toolchains["@bzd_rules_nodejs//nodejs:toolchain_type"].executable.node

    # Handle coverage.
    if ctx.configuration.coverage_enabled:
        locations = {
            ctx.attr._coverage_executor: "coverage",
            executor: "executor",
            main: "main",
        }
        command = "{coverage} {executor} {main} $@ && cp \"coverage/lcov.info\" \"$COVERAGE_OUTPUT_FILE\""
        providers = [
            coverage_common.instrumented_files_info(
                ctx,
                source_attributes = ["srcs"],
                extensions = ["js", "cjs", "mjs"],
            ),
        ]
    else:
        locations = {
            executor: "executor",
            main: "main",
        }
        command = "{executor} {main} $@"
        providers = []

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = locations,
        output = ctx.outputs.executable,
        command = command,
        extra_runfiles = [bzd_package_to_runfiles(ctx, package)],
    ), package] + providers

bzd_nodejs_binary = rule(
    doc = "NodeJs binary executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = COMMON_EXEC_ATTRS,
    executable = True,
    toolchains = ["@bzd_rules_nodejs//nodejs:toolchain_type"],
    cfg = _bzd_nodejs_transition,
)

_bzd_nodejs_test = rule(
    doc = "NodeJs test executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = COMMON_EXEC_ATTRS,
    executable = True,
    test = True,
    toolchains = ["@bzd_rules_nodejs//nodejs:toolchain_type"],
    cfg = _bzd_nodejs_transition,
)

def bzd_nodejs_test(name, tags = [], **kwargs):
    _bzd_nodejs_test(
        name = name,
        # Test rules cannot run outside of the host because of the symlinks in node_modules.
        tags = tags + ["no-remote"],
        **kwargs
    )
