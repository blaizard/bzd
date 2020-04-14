load("//tools/bazel.build:binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel.build/rules:package.bzl", "BzdPackageFragment")

BzdNodeJsInstallProvider = provider(fields = ["package_json", "node_modules", "aliases"])
BzdNodeJsDepsProvider = provider(fields = ["packages", "srcs", "aliases"])

"""
Merge providers of types BzdNodeJsDepsProvider together
"""

def _bzd_nodejs_deps_provider_merge(iterable, ctx = None):
    srcs = depset(transitive = [it[BzdNodeJsDepsProvider].srcs for it in iterable])
    packages = {}
    aliases = {}

    # Merge context information first
    if ctx:
        srcs = depset(srcs.to_list(), transitive = [f.files for f in ctx.attr.srcs])
        packages = dict(ctx.attr.packages)
        aliases = {ctx.attr.alias: ctx.label.package} if ctx.attr.alias else {}

    provider = BzdNodeJsDepsProvider(
        srcs = srcs,
        packages = packages,
        aliases = aliases,
    )

    for it in iterable:
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

def _bzd_nodejs_deps_aspect_impl(target, ctx):
    if ctx.rule.kind == "bzd_nodejs_library":
        return []
    return [_bzd_nodejs_deps_provider_merge(ctx.rule.attr.deps)]

"""
Aspects to gather data from bzd depedencies.
"""
bzd_nodejs_deps_aspect = aspect(
    implementation = _bzd_nodejs_deps_aspect_impl,
    attr_aspects = ["deps"],
)

"""
A library contains all depdencies used for this target.
"""

def _bzd_nodejs_library_impl(ctx):
    return [DefaultInfo(), _bzd_nodejs_deps_provider_merge(ctx.attr.deps, ctx = ctx)]

bzd_nodejs_library = rule(
    implementation = _bzd_nodejs_library_impl,
    attrs = {
        "alias": attr.string(
            doc = "Name of the alias, available in the form [name], for the current directory.",
        ),
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Source files",
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
    },
)

"""
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
"""

def _bzd_nodejs_install_impl(ctx):
    depsProvider = _bzd_nodejs_deps_provider_merge(ctx.attr.deps)

    # --- Generate the package.json file
    package_json = ctx.actions.declare_file("package.json")

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
    node_modules = ctx.actions.declare_directory("node_modules")
    ctx.actions.run(
        inputs = [package_json],
        outputs = [node_modules],
        progress_message = "Updating package(s) for {}".format(ctx.label),
        arguments = ["--cwd", package_json.dirname, "install", "--silent", "--non-interactive"],
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

bzd_nodejs_install = rule(
    implementation = _bzd_nodejs_install_impl,
    attrs = {
        "target_name": attr.label(
            doc = "The name of target to which this rule is associated.",
        ),
        "deps": attr.label_list(
            aspects = [bzd_nodejs_deps_aspect],
            allow_files = True,
            doc = "Dependencies",
        ),
        "_package_json_template": attr.label(
            default = Label("//tools/bazel.build/rules/assets/nodejs:package_json_template"),
            allow_single_file = True,
        ),
    },
    toolchains = ["//tools/bazel.build/toolchains/nodejs:toolchain_type"],
)

"""
Implementation of the executor
"""
def _bzd_nodejs_exec_impl(ctx, is_test):
    # Retrieve install artefacts generated by ctx.attr.install
    node_modules = ctx.attr.install[BzdNodeJsInstallProvider].node_modules
    package_json = ctx.attr.install[BzdNodeJsInstallProvider].package_json

    # Retrieve source files
    srcs = ctx.attr.install[BzdNodeJsDepsProvider].srcs.to_list()

    # Gather toolchain executable
    toolchain_executable = ctx.toolchains["//tools/bazel.build/toolchains/nodejs:toolchain_type"].executable

    result = [
        sh_binary_wrapper_impl(
            ctx = ctx,
            binary = toolchain_executable.node,
            output = ctx.outputs.executable,
            command = "{{binary}} --preserve-symlinks --preserve-symlinks-main \"{}\" $@".format(ctx.file.main.path),
            extra_runfiles = [node_modules, package_json] + srcs,
            symlinks = {
                "node_modules": node_modules,
                "package.json": package_json,
            },
        )
    ]

    if not is_test:
        result += [BzdPackageFragment(
            files = srcs,
            files_remap = {
                "node_modules": node_modules,
                "package.json": package_json,
            },
        )]

    return result

"""
NodeJs web application executor
"""

def _bzd_nodejs_binary_impl(ctx):
    return _bzd_nodejs_exec_impl(ctx, is_test = False)

_bzd_nodejs_binary = rule(
    implementation = _bzd_nodejs_binary_impl,
    attrs = {
        "main": attr.label(
            mandatory = True,
            allow_single_file = True,
            cfg = "target",
        ),
        "install": attr.label(
            mandatory = True,
            cfg = "target",
        ),
    },
    executable = True,
    toolchains = ["//tools/bazel.build/toolchains/nodejs:toolchain_type"],
)

"""
NodeJs web application tester
"""

def _bzd_nodejs_test_impl(ctx):
    return _bzd_nodejs_exec_impl(ctx, is_test = True)

_bzd_nodejs_test = rule(
    implementation = _bzd_nodejs_test_impl,
    attrs = {
        "main": attr.label(
            mandatory = True,
            allow_single_file = True,
            cfg = "target",
        ),
        "install": attr.label(
            mandatory = True,
            cfg = "target",
        ),
    },
    test = True,
    toolchains = ["//tools/bazel.build/toolchains/nodejs:toolchain_type"],
)

def bzd_nodejs_binary(name, main, deps = [], visibility = [], tags = [], **kwargs):
    # Create a library with the sources and packages
    bzd_nodejs_library(
        name = name + ".library",
        tags = ["nodejs"] + tags,
        **kwargs
    )

    # Gather dependencies and install the packages
    bzd_nodejs_install(
        name = name + ".install",
        deps = deps + [name + ".library"],
        tags = ["nodejs"] + tags,
    )

    _bzd_nodejs_binary(
        name = name,
        main = main,
        install = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )
