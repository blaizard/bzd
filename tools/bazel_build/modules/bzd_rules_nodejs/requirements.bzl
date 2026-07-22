"""Module extension for NodeJs toolchains."""

load("@bzd_lib//:defs.bzl", "bzd_repository_maker")
load("@bzd_platforms//:defs.bzl", "to_al", "to_isa")

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

def _canonical_name_to_url(canonical_name: str) -> str:
    """Convert a canonical name to a URL."""

    module_name = canonical_name.rsplit("@", 1)[0]
    basename = module_name.split("/")[-1]
    version = canonical_name.rsplit("@", 1)[1]
    return "https://registry.npmjs.org/{module_name}/-/{basename}-{version}.tgz".format(
        module_name = module_name,
        basename = basename,
        version = version,
    )

def _requirement_repository_impl(repository_ctx):
    url = _canonical_name_to_url(repository_ctx.attr.canonical_name)
    repository_ctx.download_and_extract(
        url,
        output = "srcs",
        integrity = repository_ctx.attr.integrity,
        strip_components = 1,
    )

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
load("@bazel_skylib//rules/directory:directory.bzl", "directory")
load("@bazel_skylib//rules/directory:subdirectory.bzl", "subdirectory")
load("{defs}", "bzd_nodejs_package")

directory(
    name = "root",
    srcs = glob(["srcs/**"]),
)

subdirectory(
    name = "srcs",
    parent = ":root",
    path = "srcs",
)

bzd_nodejs_package(
    name = "package",
    module_name = "{module_name}",
    canonical_name = "{canonical_name}",
    srcs = ":srcs",
    deps = {{
{dependencies}
    }},
    visibility = ["//visibility:public"],
)
""".format(
            defs = Label("//nodejs:defs.bzl"),
            module_name = _module_name_from_package(repository_ctx.attr.canonical_name),
            canonical_name = repository_ctx.attr.canonical_name,
            dependencies = ",\n".join(dependencies),
        ),
    )

requirement_repository = repository_rule(
    implementation = _requirement_repository_impl,
    attrs = {
        "canonical_name": attr.string(mandatory = True),
        "dependencies": attr.string_list(mandatory = True),
        "integrity": attr.string(mandatory = True),
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
                        integrity = metadata["integrity"],
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
