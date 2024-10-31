"""NodeJs Web binary rule."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_package//:defs.bzl", "BzdPackageFragmentInfo", "bzd_package_prefix_from_file")
load("//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo", "bzd_nodejs_install")

def _bzd_nodejs_transition_impl(_settings, _attr):
    return {"//:build_type": "nodejs_web"}

# Transition to notify the dependency graph that this is a `nodejs_web` build.
_bzd_nodejs_transition = transition(
    implementation = _bzd_nodejs_transition_impl,
    inputs = [],
    outputs = ["//:build_type"],
)

def _bzd_nodejs_web_exec_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//nodejs:toolchain_type"].executable

    vite_config = ctx.actions.declare_file("vite.config.mjs", sibling = install.package_json)
    ctx.actions.symlink(
        output = vite_config,
        target_file = ctx.file._vite_config,
    )

    # Add static files, all files under "/public" are copied into the bundle.
    statistics = []
    for static, path in ctx.attr.static.items():
        files_to_link = static[DefaultInfo].files.to_list()
        if len(files_to_link) != 1:
            fail("There can be only a single file for {}".format(static))
        symlink = ctx.actions.declare_file("public/" + path, sibling = install.package_json)
        ctx.actions.symlink(
            output = symlink,
            target_file = files_to_link[0],
        )
        statistics.append(symlink)

    # Create the index.html
    index = ctx.actions.declare_file("index.html", sibling = install.package_json)
    ctx.actions.write(
        output = index,
        content = """
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <meta http-equiv="expires" content="0">
    {heads}
  </head>
  <body>
    <div id="app"></div>
    <script type="module" src="{main}"></script>
  </body>
</html>
""".format(
            heads = "\n".join(ctx.attr.heads),
            main = ctx.file.main.short_path,
        ),
    )

    # Inject the scss config.
    config_scss = ctx.actions.declare_file("config.scss", sibling = install.package_json)
    ctx.actions.symlink(
        output = config_scss,
        target_file = ctx.file.config_scss,
    )

    bundle = ctx.actions.declare_directory("{}.bundle".format(ctx.label.name))
    ctx.actions.run(
        inputs = depset([vite_config, index, config_scss] + statistics, transitive = [install.files]),
        outputs = [bundle],
        arguments = [
            "{}/node_modules/vite/bin/vite".format(vite_config.dirname),
            "build",
            "--outDir",
            "../{}".format(bundle.basename),
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
    )

    return [
        sh_binary_wrapper_impl(
            ctx = ctx,
            locations = {
                ctx.attr._web_server: "binary",
                bundle: "bundle",
            },
            output = ctx.outputs.executable,
            command = "{binary} {bundle} $@",
        ),
        BzdPackageFragmentInfo(
            files = {
                bzd_package_prefix_from_file(bundle): depset([bundle]),
            },
        ),
    ]

_bzd_nodejs_web_binary = rule(
    doc = "NodeJs web application executor.",
    implementation = _bzd_nodejs_web_exec_impl,
    attrs = {
        "config_scss": attr.label(
            allow_single_file = True,
            doc = "Create a config.scss file at the root containing the content of this file.",
        ),
        "heads": attr.string_list(
            doc = "elements to add to the <head> tag.",
        ),
        "install": attr.label(
            mandatory = True,
            providers = [BzdNodeJsInstallInfo],
        ),
        "main": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "static": attr.label_keyed_string_dict(
            doc = "Static files to be added to the bundle.",
            default = {
                "//nodejs/static:robots.txt": "robots.txt",
            },
            allow_files = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = "@bazel_tools//tools/allowlists/function_transition_allowlist",
        ),
        "_build": attr.label(
            default = "@bzd_lib//settings/build",
        ),
        "_metadata_json": attr.label(
            default = Label("//nodejs/metadata:metadata_nodejs_web.json"),
            allow_single_file = True,
        ),
        "_vite_config": attr.label(
            default = Label("//toolchain/vite:vite.config.js"),
            allow_single_file = True,
        ),
        "_web_server": attr.label(
            executable = True,
            cfg = "target",
            default = Label("@bzd_lib//:web_server"),
        ),
    },
    cfg = _bzd_nodejs_transition,
    executable = True,
    toolchains = ["//nodejs:toolchain_type"],
)

def bzd_nodejs_web_binary(name, srcs = [], packages = [], deps = [], apis = [], **kwargs):
    """Create a web application with NodeJs.

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
            Label("@nodejs_deps//:vitejsplugin-vue"),
            Label("@nodejs_deps//:sass"),
            Label("@nodejs_deps//:terser"),
            Label("@nodejs_deps//:vite"),
        ] + packages,
        deps = deps,
    )

    _bzd_nodejs_web_binary(
        name = name,
        install = name + ".install",
        **kwargs
    )
