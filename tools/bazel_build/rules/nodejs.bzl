load("@utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//tools/bazel_build/rules:package.bzl", "BzdPackageFragment", "BzdPackageMetadataFragment", "bzd_package")
load("@io_bazel_rules_docker//container:container.bzl", "container_image", "container_push")

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
        tools = []
        for tool in ctx.attr.tools:
            tools += tool[DefaultInfo].default_runfiles.files.to_list()
        srcs = depset(srcs.to_list(), transitive = [f.files for f in ctx.attr.srcs])
        data = depset(data.to_list() + tools, transitive = [f.files for f in ctx.attr.data])
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
    "tools": attr.label_list(
        doc = "Additional tools to be added to the runfile",
        cfg = "target",
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

    manager_args = ["--cwd", package_json.dirname, "install", "--network-timeout=300000000", "--silent", "--non-interactive", "--ignore-optional", "--ignore-engines", "--prefer-offline"]

    # Set the cache directory
    cache_path = ctx.attr._cache[BuildSettingInfo].value
    if cache_path:
        manager_args += ["--cache-folder", "{}/yarn".format(cache_path)]

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//tools/bazel_build/toolchains/nodejs:toolchain_type"].executable

    # This will create and populate the node_modules directly in the output directory
    node_modules = ctx.actions.declare_directory("{}.nodejs_install/node_modules".format(ctx.label.name))
    yarn_lock_json = ctx.actions.declare_file("{}.nodejs_install/yarn.lock".format(ctx.label.name))

    ctx.actions.run(
        inputs = [package_json],
        outputs = [node_modules, yarn_lock_json],
        progress_message = "Updating package(s) for {}".format(ctx.label),
        arguments = manager_args,
        mnemonic = "NodejsUpdate",
        executable = toolchain_executable.manager.files_to_run,
    )

    # --- Fill in the metadata

    metadata = ctx.actions.declare_file("{}.nodejs_install/metadata.json".format(ctx.label.name))
    ctx.actions.run(
        inputs = [package_json, yarn_lock_json, node_modules],
        outputs = [metadata],
        progress_message = "Generating manifest for {}".format(ctx.label),
        mnemonic = "NodejsMetadata",
        arguments = [
            "--package_json",
            package_json.path,
            "--yarn_lock",
            yarn_lock_json.path,
            metadata.path,
        ],
        executable = ctx.attr._metadata.files_to_run,
    )

    # Return the provides (including outputs and dependencies)
    return [
        DefaultInfo(files = depset([package_json, node_modules])),
        BzdPackageMetadataFragment(manifests = [metadata]),
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
        default = Label("//tools/bazel_build/rules/assets/nodejs:package_json_template"),
        allow_single_file = True,
    ),
    "_metadata": attr.label(
        default = Label("//tools/bazel_build/rules/assets/nodejs:metadata"),
        cfg = "exec",
        executable = True,
    ),
    "_cache": attr.label(
        default = "//tools/bazel_build/settings/cache",
    ),
})

bzd_nodejs_install = rule(
    implementation = _bzd_nodejs_install_impl,
    attrs = _INSTALL_ATTRS,
    toolchains = ["//tools/bazel_build/toolchains/nodejs:toolchain_type"],
)

# ---- bzd_nodejs_aliases_symlinks

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
    "dep": attr.label(
        mandatory = True,
    ),
    "_metadata_json": attr.label(
        default = Label("//tools/bazel_build/rules/assets/nodejs:metadata_json"),
        allow_single_file = True,
    ),
}

"""
Implementation of the executor
"""

def _bzd_nodejs_exec_impl(ctx, is_test):
    # Retrieve install artefacts generated by ctx.attr.dep
    node_modules = ctx.attr.dep[BzdNodeJsInstallProvider].node_modules
    package_json = ctx.attr.dep[BzdNodeJsInstallProvider].package_json
    aliases = ctx.attr.dep[BzdNodeJsInstallProvider].aliases

    # Retrieve source files and data
    srcs = ctx.attr.dep[BzdNodeJsDepsProvider].srcs.to_list()
    data = ctx.attr.dep[BzdNodeJsDepsProvider].data.to_list()

    # Gather toolchain executable
    toolchain_executable = ctx.toolchains["//tools/bazel_build/toolchains/nodejs:toolchain_type"].executable

    # Run the node process
    command = """
    export BZD_RULE=nodejs
    {{binary}}"""

    # Add prefix command to support code coverage
    isCoverage = ctx.configuration.coverage_enabled
    if isCoverage:
        command += " \"{{root}}/node_modules/c8/bin/c8\" --reporter lcov --reporter text --allowExternal {{binary}}"

    if is_test:
        command += " \"{{root}}/node_modules/mocha/bin/mocha\" \"{}\""
    else:
        command += " \"{}\" $@"

    if isCoverage:
        command += " && cp \"coverage/lcov.info\" \"$COVERAGE_OUTPUT_FILE\""

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

    if isCoverage:
        result.append(
            coverage_common.instrumented_files_info(
                ctx,
                source_attributes = ["srcs"],
                extensions = ["js", "cjs", "mjs"],
            ),
        )

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

        result.append(BzdPackageMetadataFragment(
            manifests = [ctx.file._metadata_json],
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
    toolchains = ["//tools/bazel_build/toolchains/nodejs:toolchain_type"],
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
        dep = name + ".install",
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
    toolchains = ["//tools/bazel_build/toolchains/nodejs:toolchain_type"],
)

def bzd_nodejs_test(name, main, deps = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs"] + tags,
        deps = deps + [
            "//tools/bazel_build/rules:mocha",
        ],
        **kwargs
    )

    _bzd_nodejs_test(
        name = name,
        main = main,
        dep = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )

# ---- Docker Package ----

def bzd_nodejs_docker(name, deps, cmd, base = "@docker_image_nodejs//image", include_metadata = False, deploy = {}):
    bzd_package(
        name = "{}.package".format(name),
        tags = ["nodejs"],
        deps = deps,
        include_metadata = include_metadata,
    )

    root_directory = "/bzd/bin"
    map_to_directory = {dir_name: "{}/{}".format(root_directory, dir_name) for dir_name in deps.values()}

    symlinks = {}
    if include_metadata:
        symlinks["metadata.json"] = "{}/metadata.json".format(root_directory)

    container_image(
        name = name,
        base = base,
        cmd = [
            "node",
            "--experimental-json-modules",
        ] + [item.format(**map_to_directory) for item in cmd],
        symlinks = symlinks,
        directory = root_directory,
        env = {
            "NODE_ENV": "production",
        },
        stamp = "@io_bazel_rules_docker//stamp:always",
        tags = ["nodejs"],
        tars = [
            "{}.package".format(name),
        ],
    )

    # Add deploy rules if any
    for rule_name, url in deploy.items():
        registry = url.split("/")[0]
        repository = "/".join(url.split("/")[1:])

        container_push(
            name = rule_name,
            format = "Docker",
            image = ":{}".format(name),
            registry = registry,
            repository = repository,
            tag = "latest",
            tags = [
                "docker",
            ],
        )
