"""CC Toolchains."""

load("@bazel_skylib//lib:sets.bzl", "sets")
load("//cc:flags.bzl", "COPTS_CLANG", "COPTS_CLANG_COVERAGE", "COPTS_CLANG_DEV", "COPTS_CLANG_PROD", "COPTS_GCC", "COPTS_GCC_COVERAGE", "COPTS_GCC_DEV", "COPTS_GCC_PROD", "LINKOPTS_CLANG", "LINKOPTS_CLANG_COVERAGE", "LINKOPTS_GCC", "LINKOPTS_GCC_COVERAGE")

def package_path_from_label(name):
    """Convert the given label name its its package path.

    Args:
        name: The label string to be used.

    Returns:
        The path of the package corresponding to this label.
    """

    label = Label(name)
    if label.workspace_root:
        return label.workspace_root + "/" + label.package
    return label.package

def _impl(repository_ctx):
    # Create a UID
    uid = repository_ctx.name.replace("~", "-").replace("+", "-")

    # Create the loads statements.
    loads = []
    for repo, interfaces in repository_ctx.attr.loads.items():
        uniq = sets.to_list(sets.make(interfaces))
        loads.append("load(\"{}\", {})".format(repo, ", ".join(["\"{}\"".format(i) for i in uniq])))

    # Build the binary toolchain arguments
    binary_kwargs = []
    binary_kwargs.append("build = [{}],".format(", ".join(["\"{}\"".format(build) for build in repository_ctx.attr.app_build])))
    binary_kwargs.append("metadata = [{}],".format(", ".join(["\"{}\"".format(metadata) for metadata in repository_ctx.attr.app_metadata])))
    if not repository_ctx.attr.app_executors:
        fail("Toolchain is missing executors.")
    binary_kwargs.append("executors = {},".format(repository_ctx.attr.app_executors))

    # Set the default values of the binaries
    binaries = {
        "ar": "/usr/bin/false",
        "as": "/usr/bin/false",
        "cc": "/usr/bin/false",
        "cov": "/usr/bin/false",
        "cpp": "/usr/bin/false",
        "ld": "/usr/bin/false",
        "nm": "/usr/bin/false",
        "objcopy": "/usr/bin/false",
        "objdump": "/usr/bin/false",
        "strip": "/usr/bin/false",
    }
    binaries.update(repository_ctx.attr.binaries)

    # Create aliases.
    alias_template = "alias(name = \"{}\", actual = \"{}\")"
    aliases = dict(
        {key: value for key, value in binaries.items() if not value.startswith("/")},
        **repository_ctx.attr.aliases
    )

    build_substitutions = {
        "%{aliases}": "\n".join([alias_template.format(k, v) for k, v in aliases.items()]),
        "%{ar}": binaries["ar"],
        "%{as}": binaries["as"],
        "%{binary_kwargs}": "\n".join(binary_kwargs),
        "%{builtin_include_directories}": "\n".join(["'{}',".format(t) for t in repository_ctx.attr.builtin_include_directories]),
        "%{cc}": binaries["cc"],
        "%{compile_dev_flags}": "\n".join(
            ["'{}',".format(t) for t in repository_ctx.attr.compile_dev_flags],
        ),
        "%{compile_flags}": "\n".join(
            ["'-isystem', '{}',".format(package_path_from_label(t)) for t in repository_ctx.attr.system_directories] +
            ["'-isystem', '{}',".format(t) for t in repository_ctx.attr.host_system_directories] +
            ["'{}',".format(t) for t in repository_ctx.attr.compile_flags],
        ),
        "%{compile_prod_flags}": "\n".join(
            ["'{}',".format(t) for t in repository_ctx.attr.compile_prod_flags],
        ),
        "%{compiler}": repository_ctx.attr.compiler,
        "%{coverage_compile_flags}": "\n".join(["'{}',".format(t) for t in repository_ctx.attr.coverage_compile_flags]),
        "%{coverage_link_flags}": "\n".join(["'{}',".format(t) for t in repository_ctx.attr.coverage_link_flags]),
        "%{cov}": binaries["cov"],
        "%{cpp}": binaries["cpp"],
        "%{cpu}": repository_ctx.attr.cpu,
        "%{cxx_flags}": "\n".join(
            ["'{}',".format(t) for t in repository_ctx.attr.cxx_flags],
        ),
        "%{ld}": binaries["ld"],
        "%{link_flags}": "\n".join(
            ["'-L{}',".format(package_path_from_label(t)) for t in repository_ctx.attr.linker_dirs] +
            ["'{}',".format(t) for t in repository_ctx.attr.link_flags],
        ),
        "%{loads}": "\n".join(loads),
        "%{nm}": binaries["nm"],
        "%{objcopy}": binaries["objcopy"],
        "%{objdump}": binaries["objdump"],
        "%{package_name}": repository_ctx.attr.package_name,
        "%{repository_path}": "external/" + repository_ctx.name,
        "%{strip}": binaries["strip"],
        "%{sysroot}": package_path_from_label(repository_ctx.attr.sysroot) if repository_ctx.attr.sysroot else "",
        "%{uid}": uid,
    }

    # Substitute the BUILD file content.
    template = repository_ctx.read(Label("//cc:unix/template.BUILD"))
    for key, value in build_substitutions.items():
        template = template.replace(key, value)

    # Append extra build files fragments.
    for path in repository_ctx.attr.build_files:
        template += "\n" + repository_ctx.read(path)

    repository_ctx.file("BUILD", content = template, executable = False)

    # Download the data.
    repository_ctx.download_and_extract(
        url = repository_ctx.attr.urls,
        sha256 = repository_ctx.attr.sha256,
        stripPrefix = repository_ctx.attr.strip_prefix,
    )

    # Apply patches
    for patch in repository_ctx.attr.patches:
        repository_ctx.patch(patch)

_toolchain_maker_linux = repository_rule(
    doc = "Linux specific implementation of the toolchain.",
    implementation = _impl,
    attrs = {
        # Aliases
        "aliases": attr.string_dict(),
        # Execution
        "app_build": attr.string_list(),
        "app_executors": attr.string_dict(),
        "app_metadata": attr.string_list(),
        # Tools
        "binaries": attr.string_dict(),
        "build_files": attr.label_list(),
        # Run-time libraries
        "builtin_include_directories": attr.string_list(),
        "compile_dev_flags": attr.string_list(),
        "compile_flags": attr.string_list(),
        "compile_prod_flags": attr.string_list(),
        "compiler": attr.string(default = "unknown"),
        # Coverage
        "coverage_compile_flags": attr.string_list(),
        "coverage_link_flags": attr.string_list(),
        "cpu": attr.string(default = "unknown"),
        "cxx_flags": attr.string_list(),
        "default": attr.bool(default = False, doc = "Make this compiler the default one, it will be automatically selected when no compiler is given."),
        "host_system_directories": attr.string_list(),
        # Flags
        "link_flags": attr.string_list(),
        "linker_dirs": attr.label_list(),
        "loads": attr.string_list_dict(),
        # Download.
        "package_name": attr.string(default = "package"),
        "patches": attr.label_list(allow_files = True),
        "sha256": attr.string(),
        "strip_prefix": attr.string(),
        "sysroot": attr.label(),
        "system_directories": attr.label_list(),
        "tools": attr.string_dict(),
        "urls": attr.string_list(mandatory = True),
    },
)

def toolchain_maker(name, implementation, definition):
    """Creates a toolchain to be used with bazel.

    This rule also creates few important assets:
    - A toolchain: "@<id>//:toolchain"

    Args:
        name: The name of the toolchain.
        implementation: The implementation type.
        definition: The definition metadata.
    """

    if implementation == "linux_gcc":
        updated_definition = toolchain_merge({
            "compile_dev_flags": COPTS_GCC_DEV,
            "compile_flags": COPTS_GCC,
            "compile_prod_flags": COPTS_GCC_PROD,
            "coverage_compile_flags": COPTS_GCC_COVERAGE,
            "coverage_link_flags": LINKOPTS_GCC_COVERAGE,
            "link_flags": LINKOPTS_GCC,
        }, definition)

        _toolchain_maker_linux(
            name = name,
            **updated_definition
        )

    elif implementation == "linux_clang":
        updated_definition = toolchain_merge({
            "compile_dev_flags": COPTS_CLANG_DEV,
            "compile_flags": COPTS_CLANG,
            "compile_prod_flags": COPTS_CLANG_PROD,
            "coverage_compile_flags": COPTS_CLANG_COVERAGE,
            "coverage_link_flags": LINKOPTS_CLANG_COVERAGE,
            "link_flags": LINKOPTS_CLANG,
        }, definition)

        _toolchain_maker_linux(
            name = name,
            **updated_definition
        )

    else:
        fail("Unsupported toolchain type '{}'".format(implementation))

    #native.register_toolchains(
    #    "@{}//:toolchain".format(name),
    #    "@{}//:binary_toolchain".format(name),
    #)

def toolchain_merge(data1, data2):
    """Merge 2 toolchain data entries into a new one.

    Args:
        data1: The first data to be merged.
        data2: The second data to be merged.

    Returns:
        The merged data.
    """

    # Make a copy of data1 so that it can be mutated
    result = {}
    result.update(data1)

    # Populate the data2 items
    for key2, value2 in data2.items():
        if key2 in data1:
            if type(data1[key2]) != type(value2):
                fail("Trying to merge conflicting types for key '{}'.".format(key2))
            if type(value2) == "list":
                result[key2] = data1[key2] + value2
            elif data1[key2] != value2:
                fail("Trying to merge different values for key '{}'.".format(key2))
        else:
            result[key2] = value2

    return result
