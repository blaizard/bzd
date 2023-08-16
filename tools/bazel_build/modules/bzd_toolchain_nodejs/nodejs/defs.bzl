"""Rules for NodeJs."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@bzd_package//:defs.bzl", "BzdPackageFragmentInfo", "BzdPackageMetadataFragmentInfo", "bzd_package")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@rules_oci//oci:defs.bzl", "oci_image", "oci_push")

# ---- Providers

BzdNodeJsInstallInfo = provider(
    "Provider for installation information.",
    fields = ["package_json", "node_modules", "aliases"],
)
BzdNodeJsDepsInfo = provider("Provider for dependencies information", fields = ["packages", "srcs", "aliases", "data"])

# ---- Utils

def _bzd_nodejs_deps_provider_merge(deps, ctx = None):
    """Merge providers of types BzdNodeJsDepsInfo together."""

    srcs = depset(transitive = [it[BzdNodeJsDepsInfo].srcs for it in deps])
    data = depset(transitive = [it[BzdNodeJsDepsInfo].data for it in deps])
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

    provider = BzdNodeJsDepsInfo(
        srcs = srcs,
        data = data,
        packages = packages,
        aliases = dict(aliases),
    )

    for it in deps:
        # Merge packages
        for name, version in it[BzdNodeJsDepsInfo].packages.items():
            # If the version already stored is different
            if name in provider.packages and provider.packages[name] and version and provider.packages[name] != version:
                fail("Version conflict for package '{}': '{}' vs '{}'".format(name, version, provider.packages[name]))
            provider.packages[name] = version

        # Merge aliases
        for name, path in it[BzdNodeJsDepsInfo].aliases.items():
            # If the alias already stored is different
            if name in provider.aliases and provider.aliases[name] != path:
                fail("Path conflict for alias '{}': '{}' vs '{}'".format(name, path, provider.aliases[name]))
            provider.aliases[name] = path

    return provider

# ---- Aspect

def _bzd_nodejs_deps_aspect_impl(_target, ctx):
    if ctx.rule.kind == "bzd_nodejs_library":
        return []
    return [_bzd_nodejs_deps_provider_merge(deps = ctx.rule.attr.deps)]

bzd_nodejs_deps_aspect = aspect(
    doc = "Aspects to gather data from bzd depedencies.",
    implementation = _bzd_nodejs_deps_aspect_impl,
    attr_aspects = ["deps"],
)

# ---- bzd_nodejs_library

_COMMON_ATTRS = {
    "aliases": attr.string_dict(
        doc = "Name of the alias and path (relative to the workspace root).",
    ),
    "data": attr.label_list(
        allow_files = True,
        doc = "Data to be added to the runfile list",
    ),
    "deps": attr.label_list(
        aspects = [bzd_nodejs_deps_aspect],
        allow_files = True,
        doc = "Dependencies",
    ),
    "packages": attr.string_dict(
        allow_empty = True,
        doc = "Package dependencies",
    ),
    "srcs": attr.label_list(
        allow_files = True,
        doc = "Source files",
    ),
    "tools": attr.label_list(
        doc = "Additional tools to be added to the runfile",
        cfg = "target",
    ),
}

def _bzd_nodejs_library_impl(ctx):
    return [DefaultInfo(), _bzd_nodejs_deps_provider_merge(deps = ctx.attr.deps, ctx = ctx)]

bzd_nodejs_library = rule(
    doc = "A library contains all depdencies used for this target.",
    implementation = _bzd_nodejs_library_impl,
    attrs = _COMMON_ATTRS,
)

# ---- bzd_nodejs_install

def _bzd_nodejs_install_impl(ctx):
    deps_provider = _bzd_nodejs_deps_provider_merge(deps = ctx.attr.deps, ctx = ctx)

    # --- Generate the package.json file
    package_json = ctx.actions.declare_file("{}.nodejs_install/package.json".format(ctx.label.name))

    # Build the dependencies template replacement string
    dependencies = ",\n".join(["\"{}\": \"{}\"".format(name, version if version else "latest") for name, version in deps_provider.packages.items()])

    ctx.actions.expand_template(
        template = ctx.file._package_json_template,
        output = package_json,
        substitutions = {
            "{dependencies}": dependencies,
            "{name}": "{}.{}".format(ctx.label.package.replace("/", "."), ctx.attr.name),
        },
    )

    # --- Fetch and install the packages

    manager_args = ["--cwd", package_json.dirname, "install", "--network-timeout=300000000", "--silent", "--non-interactive", "--ignore-optional", "--ignore-engines", "--prefer-offline"]

    # Set the cache directory
    cache_path = ctx.attr._cache[BuildSettingInfo].value
    if cache_path:
        manager_args += ["--cache-folder", "{}/yarn".format(cache_path)]

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["@bzd_toolchain_nodejs//nodejs:toolchain_type"].executable

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
        BzdPackageMetadataFragmentInfo(manifests = [metadata]),
        BzdNodeJsInstallInfo(
            package_json = package_json,
            node_modules = node_modules,
            aliases = deps_provider.aliases,
        ),
        deps_provider,
    ]

_INSTALL_ATTRS = dict(_COMMON_ATTRS)
_INSTALL_ATTRS.update({
    "_cache": attr.label(
        default = "@bzd_toolchain_nodejs//nodejs:cache",
    ),
    "_metadata": attr.label(
        default = Label("@bzd_toolchain_nodejs//nodejs/metadata"),
        cfg = "exec",
        executable = True,
    ),
    "_package_json_template": attr.label(
        default = Label("@bzd_toolchain_nodejs//nodejs/metadata:package_json_template"),
        allow_single_file = True,
    ),
})

bzd_nodejs_install = rule(
    doc = """
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
""",
    implementation = _bzd_nodejs_install_impl,
    attrs = _INSTALL_ATTRS,
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
)

# ---- bzd_nodejs_aliases_symlinks

def bzd_nodejs_aliases_symlinks(files, aliases):
    """Generate the symlinks dictionary to be passed to root_symlinks or symlinks.

    Args:
        files: The list of files.
        aliases: The list of aliases.

    Returns:
        A dictionary of symlinks
    """

    symlinks = {}
    for name, directory in aliases.items():
        for f in files:
            if f.path.startswith(directory + "/"):
                path = "node_modules/{}/{}".format(name, f.path[len(directory) + 1:])
                symlinks[path] = f
    return symlinks

# ---- _bzd_nodejs_exec

COMMON_EXEC_ATTRS = {
    "dep": attr.label(
        mandatory = True,
    ),
    "main": attr.label(
        mandatory = True,
        allow_single_file = True,
    ),
    "_metadata_json": attr.label(
        default = Label("@bzd_toolchain_nodejs//nodejs/metadata:metadata_json"),
        allow_single_file = True,
    ),
}

def _bzd_nodejs_exec_impl(ctx, is_test):
    """Implementation of the executor."""

    # Retrieve install artefacts generated by ctx.attr.dep
    node_modules = ctx.attr.dep[BzdNodeJsInstallInfo].node_modules
    package_json = ctx.attr.dep[BzdNodeJsInstallInfo].package_json
    aliases = ctx.attr.dep[BzdNodeJsInstallInfo].aliases

    # Retrieve source files and data
    srcs = ctx.attr.dep[BzdNodeJsDepsInfo].srcs.to_list()
    data = ctx.attr.dep[BzdNodeJsDepsInfo].data.to_list()

    # Gather toolchain executable
    toolchain_executable = ctx.toolchains["@bzd_toolchain_nodejs//nodejs:toolchain_type"].executable

    # Run the node process
    command = """
    export BZD_RULE=nodejs
    {binary}"""

    # Add prefix command to support code coverage
    is_coverage = ctx.configuration.coverage_enabled
    if is_coverage:
        command += " \"{root}/node_modules/c8/bin/c8\" --reporter lcov --reporter text --allowExternal {binary}"

    if is_test:
        command += " \"{root}/node_modules/mocha/bin/mocha\" \"{path}\""
    else:
        command += " \"{path}\" $@"

    if is_coverage:
        command += " && cp \"coverage/lcov.info\" \"$COVERAGE_OUTPUT_FILE\""

    # Generate the symlinks for the aliases
    symlinks = bzd_nodejs_aliases_symlinks(files = srcs, aliases = aliases)

    result = [
        sh_binary_wrapper_impl(
            ctx = ctx,
            locations = {
                toolchain_executable.node: "binary",
                ctx.attr.main: "path",
            },
            output = ctx.outputs.executable,
            command = command,
            extra_runfiles = [node_modules, package_json] + srcs + data,
            root_symlinks = {
                "node_modules": node_modules,
                "package.json": package_json,
            },
            symlinks = symlinks,
        ),
    ]

    if is_coverage:
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

        result.append(BzdPackageFragmentInfo(
            files = srcs,
            files_remap = files_remap,
        ))

        result.append(BzdPackageMetadataFragmentInfo(
            manifests = [ctx.file._metadata_json],
        ))

    return result

# ---- _bzd_nodejs_binary

def _bzd_nodejs_binary_impl(ctx):
    return _bzd_nodejs_exec_impl(ctx, is_test = False)

_bzd_nodejs_binary = rule(
    doc = "NodeJs web application executor.",
    implementation = _bzd_nodejs_binary_impl,
    attrs = COMMON_EXEC_ATTRS,
    executable = True,
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
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

def _bzd_nodejs_test_impl(ctx):
    return _bzd_nodejs_exec_impl(ctx, is_test = True)

_bzd_nodejs_test = rule(
    doc = "NodeJs web application tester.",
    implementation = _bzd_nodejs_test_impl,
    attrs = COMMON_EXEC_ATTRS,
    test = True,
    toolchains = ["@bzd_toolchain_nodejs//nodejs:toolchain_type"],
)

def bzd_nodejs_test(name, main, deps = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs"] + tags,
        deps = deps + [
            "@bzd_toolchain_nodejs//nodejs:mocha",
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
    """Rule for embedding a NodeJs application into Docker.

    Args:
        name: The name of the target.
        deps: The dependencies.
        cmd: The command to be used.
        base: The base image.
        include_metadata: Wether metadata shall be included.
        deploy: The deploy rules.
    """

    bzd_package(
        name = "{}.package".format(name),
        tags = ["nodejs"],
        deps = deps,
        include_metadata = include_metadata,
    )

    root_directory = "/bzd/bin"
    map_to_directory = {dir_name: "{}/{}".format(root_directory, dir_name) for dir_name in deps.values()}

    oci_image(
        name = name,
        base = base,
        cmd = [
            "node",
            "--experimental-json-modules",
        ] + [item.format(**map_to_directory) for item in cmd],
        workdir = root_directory,
        env = {
            "NODE_ENV": "production",
        },
        #stamp = "@io_bazel_rules_docker//stamp:always",
        tags = ["nodejs"],
        tars = [
            "{}.package".format(name),
        ],
    )

    # Add deploy rules if any
    for rule_name, url in deploy.items():
        oci_push(
            name = rule_name,
            image = ":{}".format(name),
            repository = url,
            remote_tags = ["latest"],
            tags = [
                "docker",
            ],
        )
