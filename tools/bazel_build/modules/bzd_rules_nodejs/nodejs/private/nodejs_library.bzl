"""NodeJs library rule."""

load("@bazel_skylib//lib:paths.bzl", "paths")
load("//nodejs:private/nodejs_package.bzl", "BzdNodeJsPackageInfo")

# ---- Providers

BzdNodeJsDepsInfo = provider(
    "Provider for dependencies information",
    fields = {
        "apis": "APIs files that these deps implements.",
        "file_locations": "Tuple of 3 entries: (original file, transpiled file, virtual path when installed).",
        "imports": "Dictionary of imports (name to path).",
        "packages": "Packages to be used.",
        "runfiles": "List of runfiles to be added to the runfiles of the target.",
    },
)

# ---- Utils

def _bzd_nodejs_merge(*providers):
    """Merge providers of types BzdNodeJsDepsInfo together.

    Args:
        *providers: The providers to be merged.

    Returns:
        A BzdNodeJsDepsInfo provider containing the merged providers.
    """

    imports = {}
    for p in providers:
        for name, path in p.imports.items():
            _add_import(imports, name, path)

    provider = BzdNodeJsDepsInfo(
        apis = depset(transitive = [p.apis for p in providers]),
        packages = depset(transitive = [p.packages for p in providers]),
        file_locations = depset(transitive = [p.file_locations for p in providers]),
        imports = imports,
        runfiles = [runfile for p in providers for runfile in p.runfiles],
    )

    return provider

def _relative_link_target(link_path, target_path):
    """Compute the relative path from the parent directory of link_path to target_path."""

    link_segments = link_path.split("/")[:-1]
    target_segments = target_path.split("/")
    common = 0
    for index in range(min(len(link_segments), len(target_segments))):
        if link_segments[index] != target_segments[index]:
            break
        common = index + 1
    return "/".join([".."] * (len(link_segments) - common) + target_segments[common:])

def _declare_directory_symlink(ctx, link_path, target_path):
    """Declare a symlink pointing to a directory."""

    symlink = ctx.actions.declare_symlink(link_path)
    ctx.actions.symlink(
        output = symlink,
        target_path = _relative_link_target(link_path, target_path),
        target_type = "directory",
    )
    return symlink

def _add_link(ctx, files, links, link_path, target_path):
    """Register a directory symlink, ensuring paths are not declared twice."""

    if link_path in links:
        if links[link_path] != target_path:
            fail("Conflicting symlink for '{}': '{}' and '{}'.".format(link_path, links[link_path], target_path))
        return
    links[link_path] = target_path
    files.append(_declare_directory_symlink(ctx, link_path, target_path))

def bzd_nodejs_make_node_modules(ctx, packages, imports, base_dir_name):
    """Generate a node_modules and a package.json file at the root of `base_dir_name`.

    Args:
        ctx: The context of the rule.
        packages: The packages to be installed.
        imports: Dictionary of imports to be added(name to path).
        base_dir_name: The name of the directory where the node_modules should be located.

    Returns:
        A struct containing the package.json file, the node_modules files and its runfiles path.
    """

    # Outputs of this rule.
    package_json = ctx.actions.declare_file("{}/package.json".format(base_dir_name))
    node_modules = "{}/node_modules".format(base_dir_name)
    node_modules_short_path = "/".join(package_json.short_path.split("/")[:-1] + ["node_modules"])

    package_json_content = {
        "imports": {"#bzd/*": "./*"} | {
            "#{}/*".format(name): "./{}/*".format(path)
            for name, path in imports.items()
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

    # The package files are extracted into individual tree artifacts, while all the links are
    # created as declared symlinks. This prevents Bazel from expanding the pnpm store symlinks.
    direct_infos = [package[BzdNodeJsPackageInfo] for package in packages]
    all_infos = depset(direct_infos, transitive = [info.transitive_infos for info in direct_infos]).to_list()

    contents = {}
    for info in all_infos:
        if info.canonical_name in contents:
            fail("Duplicate package canonical name '{}'.".format(info.canonical_name))
        contents[info.canonical_name] = ctx.actions.declare_directory(
            "{}/.store/{}/pkg".format(node_modules, info.canonical_name),
        )

    if all_infos:
        args = ctx.actions.args()
        for info in all_infos:
            args.add("--extract")
            args.add(info.store)
            args.add(contents[info.canonical_name].path)

        ctx.actions.run(
            inputs = depset([info.store for info in all_infos]),
            outputs = contents.values(),
            arguments = [args],
            progress_message = "Preparing node_modules for {}...".format(ctx.label),
            mnemonic = "NodejsNodeModulesInstall",
            executable = ctx.executable._node_modules,
        )

    # Links within the package store, so each package only sees its own dependencies.
    files = []
    links = {}
    for info in all_infos:
        for alias, dep_canonical_name in info.deps.items():
            if dep_canonical_name not in contents:
                fail("Package '{}' depends on '{}' which is not part of the transitive closure.".format(info.canonical_name, dep_canonical_name))
            _add_link(
                ctx,
                files,
                links,
                "{}/.store/{}/node_modules/{}".format(node_modules, info.canonical_name, alias),
                "{}/.store/{}/pkg".format(node_modules, dep_canonical_name),
            )

    # Top-level links.
    for info in direct_infos:
        _add_link(
            ctx,
            files,
            links,
            "{}/{}".format(node_modules, info.module_name),
            "{}/.store/{}/pkg".format(node_modules, info.canonical_name),
        )

    return struct(
        node_modules_files = depset(files + contents.values()),
        node_modules_short_path = node_modules_short_path,
        package_json = package_json,
    )

def bzd_nodejs_layout_files(ctx, file_locations, base_dir_name, copy = False):
    """Layout the sources files at the given root directory.

    Args:
        ctx: The context of the rule.
        file_locations: The files to be laid out.
        base_dir_name: The name of the directory where the node_modules should be located.
        copy: Whether to copy the files instead of symlinking them. Copies are required when Node
            resolves modules by their canonical path (without --preserve-symlinks), so that
            resolution stays rooted in this directory.

    Returns:
        The file mapping: a dictionary which key is the original file and the value is the new file.
    """

    # Map all the sources to the generated files directory.
    file_mapping = {}
    if copy:
        # The files must be real (not symlinks), otherwise Node resolves them to their canonical
        # location and loses the package.json and node_modules of this directory.
        args = ctx.actions.args()
        inputs = []
        outputs = []
        for original_file, transpiled_file, mapping in file_locations.to_list():
            output = ctx.actions.declare_file("{}/{}".format(base_dir_name, mapping))
            args.add(transpiled_file)
            args.add(output)
            inputs.append(transpiled_file)
            outputs.append(output)
            file_mapping[original_file] = output

        if outputs:
            ctx.actions.run_shell(
                inputs = inputs,
                outputs = outputs,
                arguments = [args],
                command = "while [ $# -ge 2 ]; do cp -p -- \"$1\" \"$2\"; shift 2; done",
                mnemonic = "NodejsLayoutFiles",
                progress_message = "Laying out the files of {}...".format(ctx.label),
            )
    else:
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

def bzd_nodejs_setup(ctx, file_locations, packages, imports, apis, base_dir_name, copy_files = False):
    """Setup a nodejs environment.

    Args:
        ctx: The context of the rule.
        file_locations: The files to be laid out.
        packages: The packages to be installed.
        imports: Dictionary of imports to be added(name to path).
        apis: All the APIs to be merged.
        base_dir_name: The name of the directory where the node_modules should be located.
        copy_files: Whether to copy the files instead of symlinking them.

    Returns:
        A result containing all the files created and some additional accessors.
    """

    node_modules = bzd_nodejs_make_node_modules(ctx, packages, imports, base_dir_name)
    api = bzd_nodejs_create_apis(ctx, apis, base_dir_name)
    file_mapping = bzd_nodejs_layout_files(ctx, file_locations, base_dir_name, copy = copy_files)

    return struct(
        package_json = node_modules.package_json,
        node_modules_files = node_modules.node_modules_files,
        api = api,
        file_mapping = file_mapping,
        convert_path = lambda path: "{}/{}".format(base_dir_name, path),
    )

def _file_to_path(file):
    return file.short_path.replace("../", "external/")

def _label_to_path(label):
    """Convert a label into a path."""

    parts = ["external", label.workspace_name] if label.workspace_name else []
    if label.package:
        parts.append(label.package)
    parts.append(label.name)
    return "/".join(parts)

def _add_import(imports, name, path):
    """Safely add a new import into an existing imports dictionary."""

    if name in imports and imports[name] != path:
        fail("Duplicate import name '{}' for {} and {}".format(name, imports[name], path))
    imports[name] = path

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
    "imports": attr.string_dict(
        doc = "Import name to relative path, make the import available at #<name>/<path>",
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

def _bzd_nodejs_library_impl(ctx):
    """Build a file tree at the root of `base_dir` and transpile the files if needed."""

    base_dir_name = ctx.label.name

    deps_providers = _bzd_nodejs_merge(*[d[BzdNodeJsDepsInfo] for d in ctx.attr.deps])
    runfiles = deps_providers.runfiles + [ctx.runfiles(files = ctx.files.data)] + [target[DefaultInfo].default_runfiles for target in ctx.attr.tools + ctx.attr.data]
    packages = depset(ctx.attr.packages, transitive = [deps_providers.packages])
    apis = depset(ctx.files.apis, transitive = [deps_providers.apis])
    file_locations = deps_providers.file_locations
    file_locations_srcs = [(f, f, _file_to_path(f)) for f in ctx.files.srcs]
    imports = deps_providers.imports

    for name, path in ctx.attr.imports.items():
        actual = paths.normalize("{}/../{}".format(_label_to_path(ctx.label), path))
        _add_import(imports, name, actual)

    setup = bzd_nodejs_setup(
        ctx = ctx,
        file_locations = depset(file_locations_srcs, transitive = [file_locations]),
        packages = packages.to_list(),
        imports = imports,
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
            inputs = depset(
                setup.file_mapping.values() + [setup.package_json, tsconfig],
                transitive = [setup.node_modules_files],
            ),
            outputs = [f for files in typescript.values() for f in files],
            progress_message = "Processing TypeScript for {}...".format(ctx.label),
            arguments = [
                "--declaration",
                "--project",
                tsconfig.path,
            ],
            executable = ctx.executable._tsc,
        )

    return [
        DefaultInfo(files = depset([js for js, _ in typescript.values()])),
        BzdNodeJsDepsInfo(
            runfiles = runfiles,
            packages = packages,
            apis = apis,
            file_locations = depset(transpiled_file_locations, transitive = [file_locations]),
            imports = imports,
        ),
    ]

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
