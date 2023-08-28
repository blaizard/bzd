"""NodeJs Web binary rule."""

load("@bzd_package//:defs.bzl", "BzdPackageFragmentInfo")
load("@bzd_toolchain_nodejs//nodejs:private/nodejs_install.bzl", "bzd_nodejs_install")
load("@bzd_toolchain_nodejs//nodejs:private/utils.bzl", "BzdNodeJsInstallInfo")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_nodejs_transition_impl(_settings, _attr):
    return {"@bzd_toolchain_nodejs//:build_type": "nodejs_web"}

# Transition to notify the dependency graph that this is a `nodejs_web` build.
_bzd_nodejs_transition = transition(
    implementation = _bzd_nodejs_transition_impl,
    inputs = [],
    outputs = ["@bzd_toolchain_nodejs//:build_type"],
)

def _bzd_nodejs_web_exec_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["@bzd_toolchain_nodejs//nodejs:toolchain_type"].executable

    vite_config = ctx.actions.declare_file("{}/vite.config.mjs".format(install.root_dir))
    ctx.actions.symlink(
        output = vite_config,
        target_file = ctx.file._vite_config,
    )

    index = ctx.actions.declare_file("{}/index.html".format(install.root_dir))
    ctx.actions.write(
        output = index,
        content = """
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Title</title>
  </head>
  <body>
    <div id="app"></div>
    <script type="module" src="{main}"></script>
  </body>
</html>
""".format(
            main = ctx.file.main.short_path,
        ),
    )

    bundle = ctx.actions.declare_directory("{}.bundle".format(ctx.label.name))
    ctx.actions.run(
        inputs = install.files + [vite_config, index],
        outputs = [bundle],
        arguments = [
            "{}/node_modules/vite/bin/vite".format(vite_config.dirname),
            "build",
            "--outDir",
            "../{}".format(bundle.basename),
            "--config",
            vite_config.path,
        ],
        env = {
            "BZD_ROOT_DIR": vite_config.dirname,
            "FORCE_COLOR": "1",
        },
        executable = toolchain_executable.node.files_to_run,
    )

    return [sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr._web_server: "binary",
            bundle: "bundle",
        },
        output = ctx.outputs.executable,
        command = "{binary} {bundle} $@",
    ), BzdPackageFragmentInfo()]

_bzd_nodejs_web_binary = rule(
    doc = "NodeJs web application executor.",
    implementation = _bzd_nodejs_web_exec_impl,
    attrs = {
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
            default = Label("@bzd_toolchain_nodejs//nodejs_web/metadata:metadata_json"),
            allow_single_file = True,
        ),
        "_vite_config": attr.label(
            default = Label("@bzd_toolchain_nodejs//nodejs_web/metadata:vite.config.js"),
            allow_single_file = True,
        ),
        "_web_server": attr.label(
            executable = True,
            cfg = "target",
            default = Label("@bzd_utils//:web_server"),
        ),
    },
    cfg = _bzd_nodejs_transition,
    executable = True,
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
)

def bzd_nodejs_web_binary(name, srcs = [], packages = {}, deps = [], **kwargs):
    """Create a web application with NodeJs.

    Args:
        name: The name of the target.
        srcs: The source files.
        packages: The packages to add.
        deps: The dependencies.
        **kwargs: Additional arguments to add to the rule.
    """

    bzd_nodejs_install(
        name = name + ".install",
        tags = ["manual", "nodejs"],
        srcs = srcs,
        packages = dict({
            "@vitejs/plugin-vue2": "2.2.0",
            "sass": "1.66.1",
            "typescript": "5.1.6",
            "vite": "4.4.9",
        }, **packages),
        deps = deps,
    )

    _bzd_nodejs_web_binary(
        name = name,
        install = name + ".install",
        **kwargs
    )
