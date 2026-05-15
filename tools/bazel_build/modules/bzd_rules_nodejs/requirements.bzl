"""Module extension for NodeJs toolchains."""

load("@bzd_lib//:defs.bzl", "bzd_repository_maker")
load("@bzd_platforms//:defs.bzl", "get_platform_from_os", "to_al", "to_isa")
load("@node//:defs.bzl", "node_binary", "npm_binary")

def _execute(repository_ctx, args, environment = {}):
    result = repository_ctx.execute(
        args,
        environment = environment,
    )
    if result.return_code != 0:
        fail("Unable to fetch {}: Error executing '{}': {}{}".format(repository_ctx.name, " ".join([str(arg) for arg in args]), result.stdout, result.stderr))
    return result.stdout or result.stderr

def _sanitize_repository_name(name):
    """Sanitize a packge name to make it compatible with bazel target."""

    return "".join([c if c.isalnum() else "-" for c in name.elems()]).strip("-")

def _module_name_from_package(package: str) -> str:
    """Get the module name from the raw package name."""

    # If it has an alias.
    if "@npm:" in package:
        return package.split("@npm:")[0]
    return package.rsplit("@", 1)[0]

def _canonical_name_from_package(package: str) -> str:
    """Get the canonical name from the raw package name."""

    # If it has an alias.
    if "@npm:" in package:
        return package.split("@npm:")[1]
    return package

def _repository_name_from_package(package: str) -> str:
    """Get the respository name from the raw package name."""

    canonical_name = _canonical_name_from_package(package)
    return _sanitize_repository_name("package-{}".format(canonical_name))

def _requirement_repository_impl(repository_ctx):
    maybe_platform = get_platform_from_os(repository_ctx.os)
    node_path = repository_ctx.path(node_binary[maybe_platform])
    npm_path = repository_ctx.path(npm_binary[maybe_platform])

    output = _execute(
        repository_ctx,
        [npm_path, "pack", "--json", "--pack-destination", ".", repository_ctx.attr.canonical_name],
        environment = {"PATH": str(node_path.dirname)},
    )

    archive_path = json.decode(output)[0]["filename"]

    # Create the dependency list.
    dependencies = []
    for dependency in repository_ctx.attr.dependencies:
        module_name = _module_name_from_package(dependency)
        canonical_name = _canonical_name_from_package(dependency)
        repository_name = _repository_name_from_package(dependency)
        line = "\"{}\": \"@{}//:package\"".format(module_name, repository_name)
        if canonical_name not in repository_ctx.attr.valid_dependencies:
            line = "# " + line
        dependencies.append(line)

    repository_ctx.file(
        "BUILD.bazel",
        content = """
load("{defs}", "bzd_nodejs_package")

bzd_nodejs_package(
    name = "package",
    module_name = "{module_name}",
    canonical_name = "{canonical_name}",
    archive = ":{archive_path}",
    deps = {{
{dependencies}
    }},
    visibility = ["//visibility:public"],
)
""".format(
            defs = Label("//nodejs:defs.bzl"),
            module_name = _module_name_from_package(repository_ctx.attr.canonical_name),
            canonical_name = repository_ctx.attr.canonical_name,
            archive_path = archive_path,
            dependencies = ",\n".join(dependencies),
        ),
    )

requirement_repository = repository_rule(
    implementation = _requirement_repository_impl,
    attrs = {
        "canonical_name": attr.string(mandatory = True),
        "dependencies": attr.string_list(mandatory = True),
        "valid_dependencies": attr.string_list(mandatory = True),
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
        build_file = ""
        for requirement in data:
            content = module_ctx.read(requirement)
            content_json = json.decode(content)
            packages = content_json["packages"]

            # First pass, remove all incomatible packages.
            valid_packages = {package: metadata for package, metadata in packages.items() if _is_valid_dependency(module_ctx, metadata)}

            # Second pass, create the package repositories.
            for package, metadata in valid_packages.items():
                canonical_name = _canonical_name_from_package(package)
                repository_name = _repository_name_from_package(package)

                if repository_name not in already_generated:
                    requirement_repository(
                        name = repository_name,
                        canonical_name = canonical_name,
                        dependencies = metadata.get("dependencies", []),
                        valid_dependencies = valid_packages.keys(),
                    )
                    already_generated[repository_name] = True

            # Update the build file with the top-level packages.
            for requirement_name, package in content_json["top_level"].items():
                repository_name = _repository_name_from_package(package)
                if repository_name not in already_generated:
                    fail("The top-level package '{}' is requested but not generated, this should never happen.".format(package))
                build_file += """alias(
    name = "{alias_name}",
    actual = "@{repository_name}//:package",
    visibility = ["//visibility:public"],
)
""".format(
                    alias_name = _sanitize_repository_name(requirement_name),
                    repository_name = repository_name,
                )

        bzd_repository_maker(
            name = name,
            build_file_content = build_file,
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
