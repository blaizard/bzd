"""NodeJs executable rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_package//:defs.bzl", "BzdPackageFragmentInfo", "bzd_package_prefix_from_file", "bzd_package_to_runfiles")
load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo")

_COMMON_EXEC_ATTRS = {
    "data": attr.label_list(
        allow_files = True,
        doc = "Data to be available at runtime, this will not add the data as runfiles but is simply used for trget expansion with the args parameter.",
    ),
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
    "_build": attr.label(
        default = "@bzd_lib//settings/build",
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
    runfiles = bzd_package_to_runfiles(ctx, package)

    # Look for the entry point.
    paths = {
        install.transpiled.get(ctx.file.main.short_path): "main",
    }

    # Gather toolchain executable.
    executor = ctx.attr.executor if ctx.attr.executor else ctx.toolchains["@bzd_rules_nodejs//nodejs:toolchain_type"].executable.node

    if ctx.attr._build[BuildSettingInfo].value == "prod":
        command = "export NODE_ENV=production"
    else:
        command = "export NODE_ENV=development"

    # Handle coverage.
    if ctx.configuration.coverage_enabled:
        locations = {
            ctx.attr._coverage_executor: "coverage",
            executor: "executor",
        }
        command += """
{coverage} {executor} {main} "$@"
cp \"coverage/lcov.info\" \"$COVERAGE_OUTPUT_FILE\"
"""
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
        }
        command += """
{executor} {main} "$@"
"""
        providers = []

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = locations,
        paths = paths,
        output = ctx.outputs.executable,
        command = command,
        extra_runfiles = [runfiles],
    ), package] + providers

bzd_nodejs_binary = rule(
    doc = "NodeJs binary executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = _COMMON_EXEC_ATTRS,
    executable = True,
    toolchains = ["@bzd_rules_nodejs//nodejs:toolchain_type"],
    cfg = _bzd_nodejs_transition,
)

_bzd_nodejs_test = rule(
    doc = "NodeJs test executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = _COMMON_EXEC_ATTRS,
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
