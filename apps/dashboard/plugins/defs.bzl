"""Plugins helper for the dashboard."""

load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

PluginInfo = provider(
    doc = "Provider for plugins",
    fields = {
        "deps": "dependencies",
        "entry_point": "entry point",
        "metadata": "metadata information",
        "name": "Name of the plugin",
    },
)

def _bzd_plugin_impl(ctx):
    return PluginInfo(name = ctx.attr.plugin_name, metadata = ctx.file.metadata, entry_point = ctx.file.entry_point, deps = ctx.attr.deps)

_bzd_plugin = rule(
    implementation = _bzd_plugin_impl,
    attrs = {
        "deps": attr.label(),
        "entry_point": attr.label(
            allow_single_file = True,
        ),
        "metadata": attr.label(
            allow_single_file = True,
            mandatory = True,
        ),
        "plugin_name": attr.string(
            mandatory = True,
        ),
    },
)

def bzd_plugin(name, metadata, frontend_entry_point = None, frontend_srcs = [], frontend_deps = [], frontend_packages = [], backend_entry_point = None, backend_srcs = [], backend_deps = [], backend_packages = []):
    bzd_nodejs_library(
        name = "{}.frontend.library".format(name),
        srcs = frontend_srcs + [metadata] + ([frontend_entry_point] if frontend_entry_point else []),
        deps = frontend_deps,
        packages = frontend_packages,
        visibility = ["//visibility:public"],
    )

    bzd_nodejs_library(
        name = "{}.backend.library".format(name),
        srcs = backend_srcs + [metadata] + ([backend_entry_point] if backend_entry_point else []),
        deps = backend_deps,
        packages = backend_packages,
        visibility = ["//visibility:public"],
    )

    _bzd_plugin(
        name = "{}.frontend".format(name),
        plugin_name = name,
        metadata = metadata,
        entry_point = frontend_entry_point,
        deps = ":{}.frontend.library".format(name),
        visibility = ["//visibility:public"],
    )

    _bzd_plugin(
        name = "{}.backend".format(name),
        plugin_name = name,
        metadata = metadata,
        entry_point = backend_entry_point,
        deps = ":{}.backend.library".format(name),
        visibility = ["//visibility:public"],
    )

def _bzd_plugins_gen_index_impl(ctx):
    content_imports = []
    content_entries = []

    relative_to_root = "/".join([".." for s in ctx.label.package.split("/")])

    index = 0
    for plugin in ctx.attr.plugins:
        content_imports.append("import Metadata{index} from \"{relative}/{path}\";".format(index = index, relative = relative_to_root, path = plugin[PluginInfo].metadata.path))
        entry = """
        "{name}": {{
            metadata: Metadata{index},
        """.format(name = plugin[PluginInfo].name, index = index)
        if plugin[PluginInfo].entry_point:
            entry += "module: () => import(\"{relative}/{entry_point}\"),\n".format(relative = relative_to_root, entry_point = plugin[PluginInfo].entry_point.path)
        entry += "}"
        content_entries.append(entry)
        index += 1

    out = ctx.actions.declare_file(ctx.label.name + ".mjs")
    ctx.actions.write(
        output = out,
        content = """
        // Imports
        {imports}

        // Plugin entries
        export default {{
            {entries}
        }};
        """.format(imports = "\n".join(content_imports), entries = ",\n".join(content_entries)),
    )
    return [DefaultInfo(files = depset([out]))]

_bzd_plugins_gen_index = rule(
    implementation = _bzd_plugins_gen_index_impl,
    attrs = {
        "plugins": attr.label_list(
            providers = [PluginInfo],
            mandatory = True,
        ),
    },
)

def bzd_plugins(name, plugins):
    """Plugins herlper.

    Args:
        name: The name of the plugin collection.
        plugins: The plugins.
    """

    plugins_normalized = [str(Label(plugin)) for plugin in plugins]

    _bzd_plugins_gen_index(
        name = "{}.frontend.index".format(name),
        plugins = ["{}.frontend".format(plugin) for plugin in plugins_normalized],
        visibility = ["//visibility:public"],
    )

    _bzd_plugins_gen_index(
        name = "{}.backend.index".format(name),
        plugins = ["{}.backend".format(plugin) for plugin in plugins_normalized],
        visibility = ["//visibility:public"],
    )

    bzd_nodejs_library(
        name = "{}.frontend".format(name),
        srcs = [
            "{}.frontend.index".format(name),
        ],
        deps = ["{}.frontend.library".format(plugin) for plugin in plugins_normalized],
        visibility = ["//visibility:public"],
    )

    bzd_nodejs_library(
        name = "{}.backend".format(name),
        srcs = [
            "{}.backend.index".format(name),
        ],
        deps = ["{}.backend.library".format(plugin) for plugin in plugins_normalized],
        visibility = ["//visibility:public"],
    )
