load("//tools/bazel.build:binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel.build/rules:package.bzl", "BzdPackageFragment")

# ---- Providers

BzdNodeJsInstallProvider = provider(fields = ["package_json", "node_modules", "aliases"])
BzdNodeJsDepsProvider = provider(fields = ["packages", "srcs", "aliases", "data"])

# ---- Utils

"""
Merge providers of types BzdNodeJsDepsProvider together
"""

def _bzd_nodejs_deps_provider_merge(deps, ctx = None):
    srcs = depset(transitive = [it[BzdNodeJsDepsProvider].srcs for it in deps])
    data = depset(transitive = [it[BzdNodeJsDepsProvider].data for it in deps])
    packages = {}
    aliases = {}

    # Merge context information first
    if ctx:
        srcs = depset(srcs.to_list(), transitive = [f.files for f in ctx.attr.srcs])
        data = depset(data.to_list(), transitive = [f.files for f in ctx.attr.data])
        packages = dict(ctx.attr.packages)
        aliases = ctx.attr.aliases

    provider = BzdNodeJsDepsProvider(
        srcs = srcs,
        data = data,
        packages = packages,
        aliases = dict(aliases),
    )

    for it in deps:
        # Merge packages
        for name, version in it[BzdNodeJsDepsProvider].packages.items():
            # If the version already stored is different
            if name in provider.packages and provider.packages[name] and version and provider.packages[name] != version:
                fail("Version conflict for package '{}': '{}' vs '{}'".format(name, version, provider.packages[name]))
            provider.packages[name] = version

        # Merge aliases
        for name, path in it[BzdNodeJsDepsProvider].aliases.items():
            # If the alias already stored is different
            if name in provider.aliases and provider.aliases[name] != path:
                fail("Path conflict for alias '{}': '{}' vs '{}'".format(name, path, provider.aliases[name]))
            provider.aliases[name] = path

    return provider

# ---- Aspect

def _bzd_nodejs_deps_aspect_impl(target, ctx):
    if ctx.rule.kind == "bzd_nodejs_library":
        return []
    return [_bzd_nodejs_deps_provider_merge(deps = ctx.rule.attr.deps)]

"""
Aspects to gather data from bzd depedencies.
"""
bzd_nodejs_deps_aspect = aspect(
    implementation = _bzd_nodejs_deps_aspect_impl,
    attr_aspects = ["deps"],
)

# ---- bzd_nodejs_library

"""
A library contains all depdencies used for this target.
"""

_COMMON_ATTRS = {
    "aliases": attr.string_dict(
        doc = "Name of the alias and path (relative to the workspace root).",
    ),
    "srcs": attr.label_list(
        allow_files = True,
        doc = "Source files",
    ),
    "data": attr.label_list(
        allow_files = True,
        doc = "Data to be added to the runfile list",
    ),
    "packages": attr.string_dict(
        allow_empty = True,
        doc = "Package dependencies",
    ),
    "deps": attr.label_list(
        aspects = [bzd_nodejs_deps_aspect],
        allow_files = True,
        doc = "Dependencies",
    ),
}

def _bzd_nodejs_library_impl(ctx):
    return [DefaultInfo(), _bzd_nodejs_deps_provider_merge(deps = ctx.attr.deps, ctx = ctx)]

bzd_nodejs_library = rule(
    implementation = _bzd_nodejs_library_impl,
    attrs = _COMMON_ATTRS,
)

# ---- bzd_nodejs_install

"""
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
"""

def _bzd_nodejs_install_impl(ctx):
    depsProvider = _bzd_nodejs_deps_provider_merge(deps = ctx.attr.deps, ctx = ctx)

    # --- Generate the package.json file
    package_json = ctx.actions.declare_file("{}.nodejs_install/package.json".format(ctx.label.name))

    # Build the dependencies template replacement string
    dependencies = ",\n".join(["\"{}\": \"{}\"".format(name, version if version else "latest") for name, version in depsProvider.packages.items()])

    ctx.actions.expand_template(
        template = ctx.file._package_json_template,
        output = package_json,
        substitutions = {
            "{name}": "{}.{}".format(ctx.label.package.replace("/", "."), ctx.attr.name),
            "{dependencies}": dependencies,
        },
    )

    # --- Fetch and install the packages

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//tools/bazel.build/toolchains/nodejs:toolchain_type"].executable

    # This will create and populate the node_modules directly in the output directory
    node_modules = ctx.actions.declare_directory("{}.nodejs_install/node_modules".format(ctx.label.name))
    ctx.actions.run(
        inputs = [package_json],
        outputs = [node_modules],
        progress_message = "Updating package(s) for {}".format(ctx.label),
        arguments = ["--cwd", package_json.dirname, "install", "--silent", "--no-lockfile", "--non-interactive"],
        executable = toolchain_executable.manager.files_to_run,
    )

    # Return the provides (including outputs and dependencies)
    return [
        DefaultInfo(files = depset([package_json, node_modules])),
        BzdNodeJsInstallProvider(
            package_json = package_json,
            node_modules = node_modules,
            aliases = depsProvider.aliases,
        ),
        depsProvider,
    ]

_INSTALL_ATTRS = dict(_COMMON_ATTRS)
_INSTALL_ATTRS.update({
    "_package_json_template": attr.label(
        default = Label("//tools/bazel.build/rules/assets/nodejs:package_json_template"),
        allow_single_file = True,
    ),
})

bzd_nodejs_install = rule(
    implementation = _bzd_nodejs_install_impl,
    attrs = _INSTALL_ATTRS,
    toolchains = ["//tools/bazel.build/toolchains/nodejs:toolchain_type"],
)

# ---- bzd_nodejs_node_modules_symlinks

"""
Generate the symlinks dictionary to be passed to root_symlinks or symlinks
"""

def bzd_nodejs_aliases_symlinks(files, aliases):
    symlinks = {}
    for name, directory in aliases.items():
        for f in files:
            if f.path.startswith(directory + "/"):
                path = "node_modules/{}/{}".format(name, f.path[len(directory) + 1:])
                symlinks[path] = f
    return symlinks

# ---- _bzd_nodejs_exec

COMMON_EXEC_ATTRS = {
    "main": attr.label(
        mandatory = True,
        allow_single_file = True,
    ),
    "install": attr.label(
        mandatory = True,
    ),
}

"""
Implementation of the executor
"""

def _bzd_nodejs_exec_impl(ctx, is_test):
    # Retrieve install artefacts generated by ctx.attr.install
    node_modules = ctx.attr.install[BzdNodeJsInstallProvider].node_modules
    package_json = ctx.attr.install[BzdNodeJsInstallProvider].package_json
    aliases = ctx.attr.install[BzdNodeJsInstallProvider].aliases

    # Retrieve source files and data
    srcs = ctx.attr.install[BzdNodeJsDepsProvider].srcs.to_list()
    data = ctx.attr.install[BzdNodeJsDepsProvider].data.to_list()

    # Gather toolchain executable
    toolchain_executable = ctx.toolchains["//tools/bazel.build/toolchains/nodejs:toolchain_type"].executable

    # Attempt but faced yet another issue: SyntaxError: Cannot use import statement outside a module
    command = """
    export BZD_RULE=nodejs
    {{binary}}"""
    if is_test:
        command += " \"{{root}}/node_modules/mocha/bin/mocha\" \"{}\""
    else:
        command += " \"{}\" $@"

    # Generate the symlinks for the aliases
    symlinks = bzd_nodejs_aliases_symlinks(files = srcs, aliases = aliases)

    result = [
        sh_binary_wrapper_impl(
            ctx = ctx,
            binary = toolchain_executable.node,
            output = ctx.outputs.executable,
            command = command.format(ctx.file.main.path),
            extra_runfiles = [node_modules, package_json] + srcs + data,
            root_symlinks = {
                "node_modules": node_modules,
                "package.json": package_json,
            },
            symlinks = symlinks,
        ),
    ]

    if not is_test:
        files_remap = {
            "node_modules": node_modules,
            "package.json": package_json,
        }
        files_remap.update(symlinks)

        result.append(BzdPackageFragment(
            files = srcs,
            files_remap = files_remap,
        ))

    return result

# ---- _bzd_nodejs_binary

"""
NodeJs web application executor
"""

def _bzd_nodejs_binary_impl(ctx):
    return _bzd_nodejs_exec_impl(ctx, is_test = False)

_bzd_nodejs_binary = rule(
    implementation = _bzd_nodejs_binary_impl,
    attrs = COMMON_EXEC_ATTRS,
    executable = True,
    toolchains = ["//tools/bazel.build/toolchains/nodejs:toolchain_type"],
)

def bzd_nodejs_binary(name, main, args = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs"] + tags,
        **kwargs
    )

    _bzd_nodejs_binary(
        name = name,
        main = main,
        args = args,
        install = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )

# ---- _bzd_nodejs_test

"""
NodeJs web application tester
"""

def _bzd_nodejs_test_impl(ctx):
    return _bzd_nodejs_exec_impl(ctx, is_test = True)

_bzd_nodejs_test = rule(
    implementation = _bzd_nodejs_test_impl,
    attrs = COMMON_EXEC_ATTRS,
    test = True,
    toolchains = ["//tools/bazel.build/toolchains/nodejs:toolchain_type"],
)

def bzd_nodejs_test(name, main, deps = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs"] + tags,
        deps = deps + [
            "//tools/bazel.build/rules:mocha",
        ],
        **kwargs
    )

    _bzd_nodejs_test(
        name = name,
        main = main,
        install = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )
