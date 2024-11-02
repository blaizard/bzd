"""Module extension for NodeJs toolchains."""

load("@bzd_lib//:repository_maker.bzl", "repository_maker")
load("@bzd_platforms//:defs.bzl", "get_platform_from_os", "to_al", "to_isa")
load("@node//:defs.bzl", "node_binary", "npm_binary")

def _sanitize_repository_name(name):
    return "".join([c if c.isalnum() else "-" for c in name.elems()]).strip("-")

def _execute(repository_ctx, args, environment = {}):
    result = repository_ctx.execute(
        args,
        environment = environment,
    )
    if result.return_code != 0:
        fail("Unable to fetch {}: Error executing '{}': {}{}".format(repository_ctx.name, args, result.stdout, result.stderr))
    return result.stdout or result.stderr

def _package_to_alias(package):
    """Convert a package name into an alias.

    For example, mocha@1.0 -> mocha@1.0
    but momo@npm:mocha@1.0 -> momo
    """

    if "@npm:" in package:
        return package.split("@npm:")[0]
    return package

def _requirement_repository_impl(repository_ctx):
    maybe_platform = get_platform_from_os(repository_ctx.os)
    node_path = repository_ctx.path(node_binary[maybe_platform])
    npm_path = repository_ctx.path(npm_binary[maybe_platform])

    _execute(repository_ctx, ["mkdir", repository_ctx.path("packages")])
    output = _execute(
        repository_ctx,
        [npm_path, "pack", "--json", "--pack-destination", repository_ctx.path("packages")] + repository_ctx.attr.packages.keys(),
        environment = {"PATH": str(node_path.dirname)},
    )

    packages_to_filename = {}
    for npm_data, package, integrity_expected in zip(json.decode(output), repository_ctx.attr.packages.keys(), repository_ctx.attr.packages.values()):
        integrity_actual = npm_data["integrity"]
        if integrity_expected and integrity_expected != integrity_actual:
            fail("The package '{}' doesn't have the expected integrity '{}' vs '{}'.".format(package, integrity_expected, integrity_actual))
        packages_to_filename[_package_to_alias(package)] = npm_data["filename"]

    repository_ctx.file(
        "BUILD",
        content = """
load("{defs}", "bzd_nodejs_package", "bzd_nodejs_library")

bzd_nodejs_package(
    name = "package",
    package = "{package}",
    version = "{version}",
    packages = {{
        {packages}
    }},
    visibility = ["//visibility:public"],
)
""".format(
            defs = Label("//nodejs:defs.bzl"),
            package = repository_ctx.attr.package,
            version = repository_ctx.attr.version,
            packages = ",\n".join(["\"{}\": \":packages/{}\"".format(name, path) for name, path in packages_to_filename.items()]),
        ),
    )

requirement_repository = repository_rule(
    implementation = _requirement_repository_impl,
    attrs = {
        "package": attr.string(mandatory = True),
        "packages": attr.string_dict(mandatory = True),
        "version": attr.string(mandatory = True),
    },
)

def _is_valid_dependency(module_ctx, dependency):
    """Check if a dependency is valid for this platform."""

    if "os" in dependency:
        os_expected = to_al(module_ctx.os.name)
        if not any([True for os in dependency["os"] if to_al(os) == os_expected]):
            return False
    if "cpu" in dependency:
        cpu_expected = to_isa(module_ctx.os.arch)
        if not any([True for cpu in dependency["cpu"] if to_isa(cpu) == cpu_expected]):
            return False
    return True

def _requirements_nodejs_impl(module_ctx):
    # Gather all the requirements.
    requirements = {}
    for mod in module_ctx.modules:
        for parse in mod.tags.parse:
            if parse.name not in requirements:
                requirements[parse.name] = []
            requirements[parse.name].append(parse.requirements)

    already_generated = {}
    for name, data in requirements.items():
        # Merge all requirements into one.
        merged_requirements = dict()
        for requirement in data:
            content = module_ctx.read(requirement)
            content_json = json.decode(content)
            merged_requirements.update(content_json)

        build_file = ""

        # Create repositories for each requirement.
        for requirement_name, packages in merged_requirements.items():
            repository_name = _sanitize_repository_name("package_{}_{}".format(packages["name"], packages["version"]))
            if not repository_name in already_generated:
                requirement_repository(
                    name = repository_name,
                    package = packages["name"],
                    version = packages["version"],
                    packages = {
                        dependency_name: dependency["integrity"]
                        for dependency_name, dependency in packages["dependencies"].items()
                        if _is_valid_dependency(module_ctx, dependency)
                    },
                )
            already_generated[repository_name] = True
            build_file += """alias(
    name = "{alias_name}",
    actual = "@{repository_name}//:package",
    visibility = ["//visibility:public"],
)
""".format(
                alias_name = _sanitize_repository_name(requirement_name),
                repository_name = repository_name,
            )

        repository_maker(
            name = name,
            create = {
                "BUILD": build_file,
            },
        )

requirements_nodejs = module_extension(
    implementation = _requirements_nodejs_impl,
    tag_classes = {
        "parse": tag_class(
            attrs = {
                "name": attr.string(mandatory = True),
                "requirements": attr.label(mandatory = True),
            },
        ),
    },
)
