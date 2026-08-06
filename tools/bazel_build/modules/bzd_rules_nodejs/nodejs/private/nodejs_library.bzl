"""NodeJs library rule."""

load("//nodejs:private/nodejs_package.bzl", "BzdNodeJsPackageInfo")

# ---- Providers

BzdNodeJsDepsInfo = provider(
    "Provider for dependencies information",
    fields = {
        "apis": "APIs files that these deps implements.",
        "data": "Data to be added at runtime.",
        "file_locations": "Tuple of 3 entries: (original file, transpiled file, virtual path when installed).",
        "packages": "Packages to be used.",
    },
)

# ---- Utils

def bzd_nodejs_make_provider(ctx):
    """Create a provider from a rule context."""

    tool_depsets = [tool[DefaultInfo].default_runfiles.files for tool in ctx.attr.tools]
    return BzdNodeJsDepsInfo(
        data = depset(ctx.files.data, transitive = tool_depsets),
        packages = depset(ctx.attr.packages),
        apis = depset(ctx.files.apis),
        file_locations = depset([]),
    )

def bzd_nodejs_merge(*providers):
    """Merge providers of types BzdNodeJsDepsInfo together.

    Args:
        *providers: The providers to be merged.

    Returns:
        A BzdNodeJsDepsInfo provider containing the merged providers.
    """

    provider = BzdNodeJsDepsInfo(
        data = depset(transitive = [p.data for p in providers]),
        apis = depset(transitive = [p.apis for p in providers]),
        packages = depset(transitive = [p.packages for p in providers]),
        file_locations = depset(transitive = [p.file_locations for p in providers]),
    )

    return provider

def bzd_nodejs_make_node_modules(ctx, packages, base_dir_name):
    """Generate a node_modules and a package.json file at the root of `base_dir_name`.

    Args:
        ctx: The context of the rule.
        packages: The packages to be installed.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        A tuple containing the package.json file and the node_modules directory populated.
    """

    # Outputs of this rule.
    package_json = ctx.actions.declare_file("{}/package.json".format(base_dir_name))
    node_modules = ctx.actions.declare_directory("{}/node_modules".format(base_dir_name))

    package_json_content = {
        "imports": {
            "#bzd/*": "./*",
        },
        "license": "UNLICENSED",
        "name": "{}.{}".format(ctx.label.package.replace("/", "."), ctx.attr.name),
        "private": True,
        "type": "module",
        "version": "0.0.0",
    }

    ctx.actions.write(
        output = package_json,
        content = json.encode_indent(package_json_content),
    )

    transitive_stores = depset(transitive = [package[BzdNodeJsPackageInfo].transitive_stores for package in packages])

    args = ctx.actions.args()
    args.add("--output", node_modules.path)
    args.add_all(transitive_stores, before_each = "--store")
    for package in packages:
        info = package[BzdNodeJsPackageInfo]
        args.add_all("--top-level", [info.module_name, info.canonical_name])

    ctx.actions.run(
        inputs = transitive_stores,
        outputs = [node_modules],
        arguments = [args],
        progress_message = "Preparing node_modules for {}...".format(ctx.label),
        mnemonic = "NodejsNodeModulesInstall",
        executable = ctx.executable._node_modules,
    )

    return [package_json, node_modules]

def bzd_nodejs_layout_files(ctx, file_locations, base_dir_name):
    """Layout the sources files at the given root directory.

    Args:
        ctx: The context of the rule.
        file_locations: The files to be layed out.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        The file mapping: a dictionary which key is the original file and the value is the new file.
    """

    # Map all the sources to the generated files directory.
    file_mapping = {}
    for original_file, transpiled_file, mapping in file_locations.to_list():
        # This makes all file live at the same level.
        symlink = ctx.actions.declare_file("{}/{}".format(base_dir_name, mapping))
        ctx.actions.symlink(
            output = symlink,
            target_file = transpiled_file,
        )
        file_mapping[original_file] = symlink
    return file_mapping

def bzd_nodejs_create_apis(ctx, apis, base_dir_name):
    """Generate the API at the root of the given directory.

    Args:
        ctx: The context of the rule.
        apis: All the APIs to be merged.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        The generated API file.
    """

    api = ctx.actions.declare_file("{}/api.json".format(base_dir_name))
    ctx.actions.run(
        inputs = apis,
        outputs = [api],
        progress_message = "Generating API for {}...".format(ctx.label),
        arguments = [
            "--output",
            api.path,
        ] + [f.path for f in apis.to_list()],
        executable = ctx.executable._json_merge,
    )
    return api

def bzd_nodejs_setup(ctx, file_locations, packages, apis, base_dir_name):
    """Setup a nodejs environment.

    Args:
        ctx: The context of the rule.
        file_locations: The files to be layed out.
        packages: The packages to be installed.
        apis: All the APIs to be merged.
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        A result containing all the files created and some additional accessors.
    """

    package_json, node_modules = bzd_nodejs_make_node_modules(ctx, packages, base_dir_name)
    api = bzd_nodejs_create_apis(ctx, apis, base_dir_name)
    file_mapping = bzd_nodejs_layout_files(ctx, file_locations, base_dir_name)

    return struct(
        package_json = package_json,
        node_modules = node_modules,
        api = api,
        file_mapping = file_mapping,
        convert_path = lambda path: "{}/{}".format(base_dir_name, path),
    )

def _file_to_path(file):
    return file.short_path.replace("../", "external/")

# ---- Attributes

LIBRARY_ATTRS = {
    "apis": attr.label_list(
        allow_files = True,
        doc = "APIs files.",
    ),
    "data": attr.label_list(
        allow_files = True,
        doc = "Data to be available at runtime.",
    ),
    "deps": attr.label_list(
        allow_files = True,
        doc = "Dependencies of this rule.",
        providers = [BzdNodeJsDepsInfo],
    ),
    "packages": attr.label_list(
        doc = "Package dependencies.",
        providers = [BzdNodeJsPackageInfo],
    ),
    "srcs": attr.label_list(
        allow_files = True,
        doc = "Source files.",
    ),
    "tools": attr.label_list(
        doc = "Additional tools to be added to the runfile",
        cfg = "target",
    ),
}

# ---- Rule

def bzd_nodejs_library_get_provider(ctx):
    rule_provider = bzd_nodejs_make_provider(ctx)
    return bzd_nodejs_merge(rule_provider, *[d[BzdNodeJsDepsInfo] for d in ctx.attr.deps])

def _bzd_nodejs_library_impl(ctx):
    """Build a file tree at the root of `base_dir` and transpile the files if needed."""

    base_dir_name = ctx.label.name

    deps_providers = bzd_nodejs_merge(*[d[BzdNodeJsDepsInfo] for d in ctx.attr.deps])
    tools = [tool[DefaultInfo].default_runfiles.files for tool in ctx.attr.tools]
    data = depset(ctx.files.data, transitive = [deps_providers.data] + tools)
    packages = depset(ctx.attr.packages, transitive = [deps_providers.packages])
    apis = depset(ctx.files.apis, transitive = [deps_providers.apis])
    file_locations = deps_providers.file_locations
    file_locations_srcs = [(f, f, _file_to_path(f)) for f in ctx.files.srcs]

    setup = bzd_nodejs_setup(
        ctx = ctx,
        file_locations = depset(file_locations_srcs, transitive = [file_locations]),
        packages = packages.to_list(),
        apis = apis,
        base_dir_name = base_dir_name,
    )

    # Convert TypeScript to Javascript
    typescript = {}
    transpiled_file_locations = []
    for f, _, mapping in file_locations_srcs:
        if f.basename.endswith(".ts"):
            typescript_mapping = mapping.replace(".ts", ".js")
            typescript_file = ctx.actions.declare_file(setup.convert_path(typescript_mapping))
            transpiled_file_locations.append((f, typescript_file, typescript_mapping))

            typescript_declaration_mapping = mapping.replace(".ts", ".d.ts")
            typescript_declaration_file = ctx.actions.declare_file(setup.convert_path(typescript_declaration_mapping))
            transpiled_file_locations.append((typescript_declaration_file, typescript_declaration_file, typescript_declaration_mapping))

            typescript[f] = (typescript_file, typescript_declaration_file)

        else:
            transpiled_file_locations.append((f, f, mapping))

    # If there are any typescript files to process...
    if typescript:
        tsconfig = ctx.actions.declare_file(setup.convert_path("tsconfig.json"))
        ctx.actions.symlink(
            output = tsconfig,
            target_file = ctx.file._tsconfig,
        )

        ctx.actions.run(
            inputs = setup.file_mapping.values() + [setup.package_json, setup.node_modules, tsconfig],
            outputs = [f for files in typescript.values() for f in files],
            progress_message = "Processing TypeScript for {}...".format(ctx.label),
            arguments = [
                "--declaration",
                "--project",
                tsconfig.path,
            ],
            executable = ctx.executable._tsc,
        )

    return BzdNodeJsDepsInfo(
        data = data,
        packages = packages,
        apis = apis,
        file_locations = depset(transpiled_file_locations, transitive = [file_locations]),
    )

bzd_nodejs_library = rule(
    doc = "A library contains all dependencies used for this target.",
    implementation = _bzd_nodejs_library_impl,
    attrs = LIBRARY_ATTRS | {
        "_json_merge": attr.label(
            default = Label("@bzd_lib//:json_merge"),
            cfg = "exec",
            executable = True,
        ),
        "_node_modules": attr.label(
            default = "//nodejs/private/python:node_modules",
            doc = "Setup the node_modules directory.",
            cfg = "exec",
            executable = True,
        ),
        "_tsc": attr.label(
            default = Label("//toolchain/typescript:tsc"),
            cfg = "exec",
            executable = True,
        ),
        "_tsconfig": attr.label(
            default = Label("//toolchain/typescript:tsconfig.json"),
            allow_single_file = True,
        ),
    },
    provides = [BzdNodeJsDepsInfo],
)
