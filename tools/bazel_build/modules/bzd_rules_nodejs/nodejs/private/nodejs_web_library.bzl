"""NodeJs Web library rule."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo", "bzd_nodejs_install")
load("//nodejs:private/nodejs_library.bzl", "LIBRARY_ATTRS")

def _bzd_nodejs_web_transition_impl(_settings, _attr):
    return {"//:build_type": "nodejs_web"}

# Transition to notify the dependency graph that this is a `nodejs_web` build.
# This is used to select the proper dependency in some cases.
bzd_nodejs_web_transition = transition(
    implementation = _bzd_nodejs_web_transition_impl,
    inputs = [],
    outputs = ["//:build_type"],
)

def vite_run(ctx, install, inputs, output, substitutions):
    """Run the vite cli.

    Args:
        ctx: The context of the rule.
        install: The install provider.
        inputs: The inputs to be passed to vite.
        output: The expected output.
        substitutions: Substitutions to be made in the vite config.

    Returns:
        The DefaultInfo provider containing the output.
    """

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//nodejs:toolchain_type"].executable

    vite_config = ctx.actions.declare_file("vite.config.mjs", sibling = install.package_json)
    ctx.actions.expand_template(
        output = vite_config,
        template = ctx.file._vite_config,
        substitutions = substitutions | {
            # We could also use all modules here, not only top-level ones and use only the ones
            # that are not duplicated (different versions).
            "%dedupe%": json.encode(install.modules),
        },
    )

    ctx.actions.run(
        inputs = depset([vite_config] + inputs, transitive = [install.files]),
        outputs = [output],
        arguments = [
            "{}/vite/bin/vite".format(install.node_modules.path),
            "build",
            "--logLevel",
            "warn",
            "--config",
            vite_config.path,
        ],
        env = {
            "BZD_ROOT_DIR": vite_config.dirname,
            "FORCE_COLOR": "1",
            "NODE_ENV": "production" if ctx.attr._build[BuildSettingInfo].value == "prod" else "development",
        },
        executable = toolchain_executable.node.files_to_run,
        progress_message = "Running vite for {}".format(str(ctx.label)),
    )

    return [DefaultInfo(
        files = depset([output]),
    )]

def _vite_bundle_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    # Output is:
    # - library.js
    # - library.umd.cjs
    # - library.iife.js
    library = ctx.actions.declare_file("{}.output/library.umd.cjs".format(ctx.label.name))
    return vite_run(
        ctx = ctx,
        install = install,
        inputs = [],
        output = library,
        substitutions = {
            "%main%": ctx.file.main.path,
            "%name%": ctx.attr.library_name or ctx.label.name,
            "%output%": library.path,
        },
    )

_vite_bundle = rule(
    doc = "NodeJs web library.",
    implementation = _vite_bundle_impl,
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
            default = Label("//toolchain/vite:vite.config.library.js.template"),
            allow_single_file = True,
        ),
    },
    cfg = bzd_nodejs_web_transition,
    toolchains = ["//nodejs:toolchain_type"],
)

def _bzd_nodejs_web_library_impl(name, visibility, srcs, packages, deps, apis, tools, data, **kwargs):
    """Create a web library with NodeJs."""

    _ = data  # @unused

    bzd_nodejs_install(
        name = name + ".install",
        tags = ["manual", "nodejs"],
        srcs = srcs,
        apis = apis,
        packages = [
            Label("@nodejs_deps//:vitejs-plugin-vue"),
            Label("@nodejs_deps//:postcss-preset-env"),
            Label("@nodejs_deps//:sass-embedded"),
            Label("@nodejs_deps//:terser"),
            Label("@nodejs_deps//:vite"),
        ] + packages,
        deps = deps,
        tools = tools,
    )

    _vite_bundle(
        name = name,
        install = name + ".install",
        visibility = visibility,
        **kwargs
    )

bzd_nodejs_web_library = macro(
    implementation = _bzd_nodejs_web_library_impl,
    inherit_attrs = _vite_bundle,
    attrs = {
        "install": None,
    } | LIBRARY_ATTRS,
)
