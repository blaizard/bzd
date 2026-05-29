"""NodeJs executable rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo", "bzd_nodejs_install")
load("//nodejs:private/nodejs_library.bzl", "LIBRARY_ATTRS")

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
        default = Label("//toolchain/c8"),
        executable = True,
        cfg = "exec",
    ),
    "_metadata_json": attr.label(
        default = Label("//nodejs/metadata:metadata_nodejs.json"),
        allow_single_file = True,
    ),
}

def _bzd_nodejs_transition_impl(_settings, _attr):
    return {"//:build_type": "nodejs"}

# Transition to notify the dependency graph that this is a `nodejs` build.
_bzd_nodejs_transition = transition(
    implementation = _bzd_nodejs_transition_impl,
    inputs = [],
    outputs = ["//:build_type"],
)

def _bzd_nodejs_executable_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    runfiles = ctx.runfiles(
        transitive_files = install.files,
    )

    # Look for the entry point.
    paths = {
        install.path_mapping[ctx.file.main].short_path: "main",
    }

    # Gather toolchain executable.
    executor = ctx.attr.executor if ctx.attr.executor else ctx.toolchains["//nodejs:toolchain_type"].executable.node

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

    default_info = sh_binary_wrapper_impl(
        ctx = ctx,
        locations = locations,
        paths = paths,
        output = ctx.outputs.executable,
        command = command,
        extra_runfiles = [runfiles],
    )

    return [default_info, OutputGroupInfo(
        node_modules = depset([install.node_modules]),
    )] + providers

_bzd_nodejs_binary = rule(
    doc = "NodeJs binary executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = _COMMON_EXEC_ATTRS,
    executable = True,
    toolchains = ["//nodejs:toolchain_type"],
    cfg = _bzd_nodejs_transition,
)

_bzd_nodejs_test = rule(
    doc = "NodeJs test executor.",
    implementation = _bzd_nodejs_executable_impl,
    attrs = _COMMON_EXEC_ATTRS,
    executable = True,
    test = True,
    toolchains = ["//nodejs:toolchain_type"],
    cfg = _bzd_nodejs_transition,
)

def _bzd_nodejs_binary_macro_impl(name, visibility, data, tools, deps, packages, srcs, apis, tags, **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        data = data,
        deps = deps,
        packages = packages,
        srcs = srcs,
        apis = apis,
        tags = ["nodejs", "manual"],
        tools = tools,
    )

    _bzd_nodejs_binary(
        name = name,
        data = data,
        install = name + ".install",
        tags = ["nodejs"] + (tags or []),
        visibility = visibility,
        **kwargs
    )

bzd_nodejs_binary = macro(
    implementation = _bzd_nodejs_binary_macro_impl,
    inherit_attrs = _bzd_nodejs_binary,
    attrs = {
        "executor": None,
        "install": None,
    } | LIBRARY_ATTRS,
)

def _bzd_nodejs_test_macro_impl(name, visibility, data, tools, deps, packages, srcs, apis, tags, **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        data = data,
        deps = deps,
        packages = packages,
        srcs = srcs,
        apis = apis,
        tags = ["nodejs", "manual"],
        tools = tools,
    )

    _bzd_nodejs_test(
        name = name,
        data = data,
        executor = Label("//toolchain/mocha"),
        install = name + ".install",
        tags = ["nodejs"] + (tags or []),
        visibility = visibility,
        **kwargs
    )

bzd_nodejs_test = macro(
    implementation = _bzd_nodejs_test_macro_impl,
    inherit_attrs = _bzd_nodejs_test,
    attrs = {
        "executor": None,
        "install": None,
    } | LIBRARY_ATTRS,
)
