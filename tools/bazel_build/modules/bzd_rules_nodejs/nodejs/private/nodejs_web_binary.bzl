"""NodeJs Web binary rule."""

load("@bzd_lib//:web_server_binary.bzl", "bzd_web_server_binary")
load("//nodejs:private/nodejs_install.bzl", "BzdNodeJsInstallInfo", "bzd_nodejs_install")
load("//nodejs:private/nodejs_library.bzl", "LIBRARY_ATTRS")
load("//nodejs:private/nodejs_web_library.bzl", "bzd_nodejs_web_transition", "vite_run")

def _vite_bundle_impl(ctx):
    # Retrieve the install provider
    install = ctx.attr.install[BzdNodeJsInstallInfo]

    static_files_dict = {file.short_path: file for file in ctx.files.data}
    for static, path in ctx.attr.static.items():
        files_to_link = static[DefaultInfo].files.to_list()
        if len(files_to_link) != 1:
            fail("There can be only a single file for {}".format(static))
        if path in static_files_dict:
            fail("The static file '{}' was registered twice.".format(path))
        static_files_dict[path] = files_to_link[0]

    # Add static files, all files under "/public" are copied into the bundle.
    static_files = []
    for path, file in static_files_dict.items():
        symlink = ctx.actions.declare_file("public/" + path, sibling = install.package_json)
        ctx.actions.symlink(
            output = symlink,
            target_file = file,
        )
        static_files.append(symlink)

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

    bundle = ctx.actions.declare_directory(ctx.label.name)
    return vite_run(
        ctx = ctx,
        install = install,
        inputs = [index, config_scss] + static_files,
        output = bundle,
        substitutions = {
            "%bundle%": bundle.path,
        },
    )

_vite_bundle = rule(
    doc = "NodeJs web application bundler.",
    implementation = _vite_bundle_impl,
    attrs = {
        "config_scss": attr.label(
            allow_single_file = True,
            doc = "Create a config.scss file at the root containing the content of this file.",
        ),
        "data": attr.label_list(
            doc = "Files to be added to the bundle.",
            allow_files = True,
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
            default = Label("//toolchain/vite:vite.config.binary.js"),
            allow_single_file = True,
        ),
    },
    cfg = bzd_nodejs_web_transition,
    toolchains = ["//nodejs:toolchain_type"],
)

def _bzd_nodejs_web_binary_impl(name, visibility, srcs, packages, deps, apis, tools, tags, **kwargs):
    """Create a web application with NodeJs."""

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
        name = name + ".bundle",
        install = name + ".install",
        visibility = visibility,
        **kwargs
    )

    bzd_web_server_binary(
        name = name,
        static = name + ".bundle",
        visibility = visibility,
        tags = tags,
    )

bzd_nodejs_web_binary = macro(
    implementation = _bzd_nodejs_web_binary_impl,
    inherit_attrs = _vite_bundle,
    attrs = {
        "install": None,
    } | LIBRARY_ATTRS,
)
