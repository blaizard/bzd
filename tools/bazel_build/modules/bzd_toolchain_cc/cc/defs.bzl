load("@bzd_toolchain_cc//cc:flags.bzl", "COPTS_CLANG", "COPTS_CLANG_COVERAGE", "COPTS_CLANG_DEV", "COPTS_CLANG_PROD", "COPTS_GCC", "COPTS_GCC_COVERAGE", "COPTS_GCC_DEV", "COPTS_GCC_PROD", "LINKOPTS_CLANG", "LINKOPTS_CLANG_COVERAGE", "LINKOPTS_GCC", "LINKOPTS_GCC_COVERAGE")
load("@bazel_skylib//lib:sets.bzl", "sets")

def _impl(repository_ctx):

    # Create a UID
    uid = repository_ctx.name.replace("~", "-")

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
        "as": "/usr/bin/false",
        "ar": "/usr/bin/false",
        "ld": "/usr/bin/false",
        "cc": "/usr/bin/false",
        "cov": "/usr/bin/false",
        "cpp": "/usr/bin/false",
        "nm": "/usr/bin/false",
        "objdump": "/usr/bin/false",
        "objcopy": "/usr/bin/false",
        "strip": "/usr/bin/false",
    }
    binaries.update(repository_ctx.attr.binaries)

    # Create aliases.
    alias_template = "alias(name = \"{}\", actual = \"{}\")"
    aliases = dict({key: value for key, value in binaries.items() if not value.startswith("/")},
        **repository_ctx.attr.aliases)

    build_substitutions = {
        "%{uid}": uid,
        "%{loads}": "\n".join(loads),
        "%{repository_path}": "external/" + repository_ctx.name,
        "%{cpu}": repository_ctx.attr.cpu,
        "%{compiler}": repository_ctx.attr.compiler,
        "%{ar}": binaries["ar"],
        "%{as}": binaries["as"],
        "%{cc}": binaries["cc"],
        "%{cpp}": binaries["cpp"],
        "%{cov}": binaries["cov"],
        "%{nm}": binaries["nm"],
        "%{ld}": binaries["ld"],
        "%{objcopy}": binaries["objcopy"],
        "%{objdump}": binaries["objdump"],
        "%{strip}": binaries["strip"],
        "%{builtin_include_directories}": "\n".join(["'{}',".format(t) for t in repository_ctx.attr.builtin_include_directories]),
        "%{compile_flags}": "\n".join(
            ["'-isystem', '{}',".format(t) for t in repository_ctx.attr.system_directories] +
            ["'{}',".format(t) for t in repository_ctx.attr.compile_flags],
        ),
        "%{compile_dev_flags}": "\n".join(
            ["'{}',".format(t) for t in repository_ctx.attr.compile_dev_flags],
        ),
        "%{compile_prod_flags}": "\n".join(
            ["'{}',".format(t) for t in repository_ctx.attr.compile_prod_flags],
        ),
        "%{link_flags}": "\n".join(
            ["'-L{}',".format(t) for t in repository_ctx.attr.linker_dirs] +
            ["'{}',".format(t) for t in repository_ctx.attr.link_flags],
        ),
        "%{sysroot}": repository_ctx.attr.sysroot,
        "%{aliases}": "\n".join([alias_template.format(k, v) for k, v in aliases.items()]),
        "%{binary_kwargs}": "\n".join(binary_kwargs),
        "%{coverage_compile_flags}": "\n".join(["'{}',".format(t) for t in repository_ctx.attr.coverage_compile_flags]),
        "%{coverage_link_flags}": "\n".join(["'{}',".format(t) for t in repository_ctx.attr.coverage_link_flags]),
    }

    # Substitute the BUILD file content.
    template = repository_ctx.read(Label("@bzd_toolchain_cc//cc:unix/template.BUILD"))
    for key, value in build_substitutions.items():
        template = template.replace(key, value)

    # Append extra build files fragments.
    for path in repository_ctx.attr.build_files:
        template += "\n" + repository_ctx.read(path)

    repository_ctx.file("BUILD", content=template, executable=False)

    # Download the data.
    repository_ctx.download_and_extract(
        url = repository_ctx.attr.url,
        sha256 = repository_ctx.attr.sha256,
        stripPrefix = repository_ctx.attr.strip_prefix,
    )

    # Apply patches
    for patch in repository_ctx.attr.patches:
        repository_ctx.patch(patch)

"""
Linux specific implementation of the toolchain
"""
_toolchain_maker_linux = repository_rule(
    implementation = _impl,
    attrs = {
        "default": attr.bool(default = False, doc = "Make this compiler the default one, it will be automatically selected when no compiler is given."),
        "cpu": attr.string(default = "unknown"),
        "compiler": attr.string(default = "unknown"),
        # Download.
        "loads": attr.string_list_dict(),
        "build_files": attr.label_list(),
        "url": attr.string(mandatory = True),
        "strip_prefix": attr.string(),
        "sha256": attr.string(),
        "patches": attr.label_list(allow_files=True),
        # Run-time libraries
        "builtin_include_directories": attr.string_list(),
        "system_directories": attr.string_list(),
        "linker_dirs": attr.string_list(),
        "sysroot": attr.string(),
        # Flags
        "link_flags": attr.string_list(),
        "compile_flags": attr.string_list(),
        "compile_dev_flags": attr.string_list(),
        "compile_prod_flags": attr.string_list(),
        # Coverage
        "coverage_compile_flags": attr.string_list(),
        "coverage_link_flags": attr.string_list(),
        # Aliases
        "aliases": attr.string_dict(),
        # Tools
        "binaries": attr.string_dict(),
        "tools": attr.string_dict(),
        # Execution
        "app_build": attr.string_list(),
        "app_metadata": attr.string_list(),
        "app_executors": attr.string_dict(),
    },
)

"""
Creates a toolchain to be used with bazel.

This rule also creates few important assets:
 - A toolchain: "@<id>//:toolchain"
"""

def toolchain_maker(name, implementation, definition):
    if implementation == "linux_gcc":
        updated_definition = toolchain_merge({
            "compile_dev_flags": COPTS_GCC_DEV,
            "compile_prod_flags": COPTS_GCC_PROD,
            "compile_flags": COPTS_GCC,
            "link_flags": LINKOPTS_GCC,
            "coverage_compile_flags": COPTS_GCC_COVERAGE,
            "coverage_link_flags": LINKOPTS_GCC_COVERAGE,
        }, definition)

        _toolchain_maker_linux(
            name = name,
            **updated_definition
        )

    elif implementation == "linux_clang":
        updated_definition = toolchain_merge({
            "compile_dev_flags": COPTS_CLANG_DEV,
            "compile_prod_flags": COPTS_CLANG_PROD,
            "compile_flags": COPTS_CLANG,
            "link_flags": LINKOPTS_CLANG,
            "coverage_compile_flags": COPTS_CLANG_COVERAGE,
            "coverage_link_flags": LINKOPTS_CLANG_COVERAGE,
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
    """Merge 2 toolchain data entries."""

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

def get_location(module_ctx, name):
    """Return the full name of the repository on disk."""

    return "external/" + module_ctx.path(".").basename + "~" + name
