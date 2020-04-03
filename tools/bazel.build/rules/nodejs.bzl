BzdNodeJsInstallProvider = provider(fields = ["package_json", "node_modules"])
BzdNodeJsDepsProvider = provider(fields = ["packages", "srcs"])

"""
A library contains all depdencies used for this target.
"""
def _bzd_nodejs_library_impl(ctx):
    return [DefaultInfo(), BzdNodeJsDepsProvider(
        srcs = depset(transitive = [f.files for f in ctx.attr.srcs]),
        packages = dict(ctx.attr.packages),
    )]

bzd_nodejs_library = rule(
    implementation = _bzd_nodejs_library_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Source files",
        ),
        "packages": attr.string_dict(
            allow_empty = True,
            doc = "Package dependencies",
        ),
    }
)

"""
Merge providers of types BzdNodeJsDepsProvider together
"""
def _bzd_nodejs_deps_provider_merge(iterable):
    provider = BzdNodeJsDepsProvider(
        srcs = depset(transitive = [it[BzdNodeJsDepsProvider].srcs for it in iterable]),
        packages = {},
    )
    # Merge packages
    for it in iterable:
        for name, version in it[BzdNodeJsDepsProvider].packages.items():
            if name in provider.packages and provider.packages[name] != version:
                fail("Version conflict for package '{}': '{}' vs '{}'".format(name, version, provider.packages[name]))
            provider.packages[name] = version

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
Install a NodeJs environment, dealing with the creation of the package.json
and the installation of the actual packages.
"""
def _bzd_nodejs_install_impl(ctx):

    depsProvider = _bzd_nodejs_deps_provider_merge(ctx.attr.deps)

    # --- Generate the package.json file
    package_json = ctx.actions.declare_file("package.json")

    # Build the dependencies template replacement string
    dependencies = ",\n".join(["\"{}\": \"{}\"".format(name, version) for name, version in depsProvider.packages.items()])

    ctx.actions.expand_template(
        template = ctx.file._package_json_template,
        output = package_json,
        substitutions = {
            "{name}": "{}.{}".format(ctx.label.package.replace("/", "."), ctx.attr.name),
            "{dependencies}": dependencies
        },
    )

    # --- Fetch and install the packages

    # This will create and populate the node_modules directly in the output directory
    node_modules = ctx.actions.declare_directory("node_modules")
    ctx.actions.run(
        inputs = [package_json],
        outputs = [node_modules],
        progress_message = "Updating package(s) for {}".format(ctx.label),
        arguments = ["--cwd", package_json.dirname, "install"],
        executable = ctx.executable._yarn,
    )

    # Return the provides (including outputs and dependencies)
    return [DefaultInfo(files = depset([package_json, node_modules])),
            BzdNodeJsInstallProvider(package_json = package_json, node_modules = node_modules),
            depsProvider]

bzd_nodejs_install = rule(
    implementation = _bzd_nodejs_install_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
            mandatory = True,
            doc = "Source files",
        ),
        "deps": attr.label_list(
            aspects = [bzd_nodejs_deps_aspect],
            allow_files = True,
            doc = "Dependencies",
        ),
        "_yarn": attr.label(
            executable = True,
            cfg = "host",
            allow_files = True,
            default = Label("//toolchains/nodejs/linux_x86_64_yarn:yarn"),
        ),
        "_package_json_template": attr.label(
            default = Label("//tools/bazel.build/rules/assets/nodejs:package_json_template"),
            allow_single_file = True,
        ),
    },
)
