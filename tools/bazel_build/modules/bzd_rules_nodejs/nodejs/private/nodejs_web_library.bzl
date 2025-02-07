"""NodeJs Web library rule."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo", "bzd_nodejs_install")

def _bzd_nodejs_transition_impl(_settings, _attr):
    return {"//:build_type": "nodejs_web"}

# Transition to notify the dependency graph that this is a `nodejs_web` build.
# This is used to select the proper dependency in some cases.
_bzd_nodejs_transition = transition(
    implementation = _bzd_nodejs_transition_impl,
    inputs = [],
    outputs = ["//:build_type"],
)

def _bzd_nodejs_web_library_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//nodejs:toolchain_type"].executable

    vite_config = ctx.actions.declare_file("vite.config.mjs", sibling = install.package_json)
    ctx.actions.expand_template(
        output = vite_config,
        template = ctx.file._vite_config,
        substitutions = {
            "%main%": ctx.file.main.path,
            "%name%": ctx.attr.library_name or ctx.label.name,
            "%root%": vite_config.dirname,
        },
    )

    # Output is:
    # - library.js
    # - library.umd.cjs
    # - library.iife.js
    library = ctx.actions.declare_file("{}.output/library.umd.cjs".format(ctx.label.name))
    ctx.actions.run(
        inputs = depset([vite_config], transitive = [install.files]),
        outputs = [library],
        arguments = [
            "{}/node_modules/vite/bin/vite".format(vite_config.dirname),
            "build",
            "--outDir",
            "../{}.output".format(ctx.label.name),
            "--logLevel",
            "warn",
            "--config",
            vite_config.path,
        ],
        env = {
            "FORCE_COLOR": "1",
            "NODE_ENV": "production" if ctx.attr._build[BuildSettingInfo].value == "prod" else "development",
        },
        executable = toolchain_executable.node.files_to_run,
    )

    return [
        DefaultInfo(
            files = depset([library]),
        ),
    ]

_bzd_nodejs_web_library = rule(
    doc = "NodeJs web library.",
    implementation = _bzd_nodejs_web_library_impl,
    attrs = {
        "install": attr.label(
            mandatory = True,
            providers = [BzdNodeJsInstallInfo],
        ),
        "library_name": attr.string(
            doc = "An optional name for the library, if unset, the name of the rule will be used.",
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
        "_vite_config": attr.label(
            default = Label("//toolchain/vite:vite.config.library.js"),
            allow_single_file = True,
        ),
    },
    cfg = _bzd_nodejs_transition,
    toolchains = ["//nodejs:toolchain_type"],
)

def bzd_nodejs_web_library(name, srcs = [], packages = [], deps = [], apis = [], **kwargs):
    """Create a web library with NodeJs.

    Args:
        name: The name of the target.
        srcs: The source files.
        packages: The packages to add.
        deps: The dependencies.
        apis: The API to be used.
        **kwargs: Additional arguments to add to the rule.
    """

    bzd_nodejs_install(
        name = name + ".install",
        tags = ["manual", "nodejs"],
        srcs = srcs,
        apis = apis,
        packages = [
            Label("@nodejs_deps//:sass-embedded"),
            Label("@nodejs_deps//:terser"),
            Label("@nodejs_deps//:vite"),
        ] + packages,
        deps = deps,
    )

    _bzd_nodejs_web_library(
        name = name,
        install = name + ".install",
        **kwargs
    )
