load("//tools/bazel_build/toolchains/cc:flags.bzl", "COPTS_CLANG", "COPTS_CLANG_COVERAGE", "COPTS_CLANG_DEV", "COPTS_CLANG_PROD", "COPTS_GCC", "COPTS_GCC_COVERAGE", "COPTS_GCC_DEV", "COPTS_GCC_PROD", "LINKOPTS_CLANG", "LINKOPTS_CLANG_COVERAGE", "LINKOPTS_GCC", "LINKOPTS_GCC_COVERAGE")

def _impl(ctx):
    # Absolute path of the external directory (used for linking librairies for example)
    absolute_external = ctx.execute(["pwd"], working_directory = "..").stdout.rstrip()

    alias_template = "alias(name = \"{}\", actual = \"{}\")"

    # Build the binary toolchain arguments
    binary_kwargs = []
    binary_kwargs.append("build = [{}],".format(", ".join(["\"{}\"".format(build) for build in ctx.attr.app_build])))
    binary_kwargs.append("metadata = [{}],".format(", ".join(["\"{}\"".format(metadata) for metadata in ctx.attr.app_metadata])))
    if not ctx.attr.app_executors:
        fail("Toolchain is missing executors.")
    binary_kwargs.append("executors = {},".format(ctx.attr.app_executors))

    # Generate the constraint value for the compiler.
    if ctx.attr.default:
        compiler_constraint_value = """alias(name = "compiler", actual = "@//tools/bazel_build/platforms/compiler:default")"""
    else:
        compiler_constraint_value = """constraint_value(
            name = "compiler",
            constraint_setting = "@//tools/bazel_build/platforms/compiler:compiler",
        )"""

    build_substitutions = {
        "%{compiler_constraint_value}": compiler_constraint_value,
        "%{cpu}": ctx.attr.cpu,
        "%{compiler}": ctx.attr.compiler,
        "%{ar_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.ar_files]),
        "%{as_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.as_files]),
        "%{compiler_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.compiler_files]),
        "%{linker_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.linker_files]),
        "%{objcopy_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.objcopy_files]),
        "%{strip_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.strip_files]),
        "%{dynamic_libraries_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.dynamic_libraries_files]),
        "%{static_libraries_files}": "\n".join(['"{}",'.format(t) for t in ctx.attr.static_libraries_files]),
        "%{builtin_include_directories}": "\n".join(["'{}',".format(t) for t in ctx.attr.builtin_include_directories]),
        "%{compile_flags}": "\n".join(
            ["'-isystem', '{}',".format(t) for t in ctx.attr.system_directories] +
            ["'{}',".format(t) for t in ctx.attr.compile_flags],
        ),
        "%{compile_dev_flags}": "\n".join(
            ["'{}',".format(t) for t in ctx.attr.compile_dev_flags],
        ),
        "%{compile_prod_flags}": "\n".join(
            ["'{}',".format(t) for t in ctx.attr.compile_prod_flags],
        ),
        "%{link_flags}": "\n".join(
            ["'-L{}',".format(t) for t in ctx.attr.linker_dirs] +
            ["'{}',".format(t) for t in ctx.attr.link_flags],
        ),
        "%{exec_compatible_with}": "\n".join(['"{}",'.format(t) for t in ctx.attr.exec_compatible_with]),
        "%{target_compatible_with}": "\n".join(['"{}",'.format(t) for t in ctx.attr.target_compatible_with]),
        "%{alias}": "\n".join([alias_template.format(k, v) for k, v in ctx.attr.alias.items()]),
        "%{binary_kwargs}": "\n".join(binary_kwargs),
        "%{coverage_compile_flags}": "\n".join(["'{}',".format(t) for t in ctx.attr.coverage_compile_flags]),
        "%{coverage_link_flags}": "\n".join(["'{}',".format(t) for t in ctx.attr.coverage_link_flags]),
    }

    # Apply substitutions
    build_substitutions = {k: v.replace("%{absolute_external}", absolute_external) for k, v in build_substitutions.items()}

    # Set the toolchain BUILD
    ctx.template("BUILD", Label("//tools/bazel_build/toolchains/cc:template/BUILD.template"), build_substitutions)

    # Set the default values of the binaries
    binaries = {
        "ar": "/usr/bin/ar",
        "ld": "/usr/bin/ld",
        "cov": "/usr/bin/gcov",
        "nm": "/usr/bin/nm",
        "objdump": "/usr/bin/objdump",
        "objcopy": "/usr/bin/objcopy",
        "strip": "/usr/bin/strip",
    }
    binaries.update(ctx.attr.binaries)

    # Set the binary wrappers
    ctx.template("bin/wrapper_ar", ctx.attr.template_bin_ar, {"%{ar}": binaries["ar"]})
    ctx.template("bin/wrapper_as", ctx.attr.template_bin_as, {"%{as}": binaries["as"]})
    ctx.template("bin/wrapper_cc", ctx.attr.template_bin_cc, {"%{cc}": binaries["cc"]})
    ctx.template("bin/wrapper_cpp", ctx.attr.template_bin_cpp, {"%{cpp}": binaries["cpp"]})
    ctx.template("bin/wrapper_ld", ctx.attr.template_bin_ld, {"%{ld}": binaries["ld"]})
    ctx.template("bin/wrapper_cov", ctx.attr.template_bin_cov, {"%{cov}": binaries["cov"]})
    ctx.template("bin/wrapper_nm", ctx.attr.template_bin_nm, {"%{nm}": binaries["nm"]})
    ctx.template("bin/wrapper_objdump", ctx.attr.template_bin_objdump, {"%{objdump}": binaries["objdump"]})
    ctx.template("bin/wrapper_objcopy", ctx.attr.template_bin_objcopy, {"%{objcopy}": binaries["objcopy"]})
    ctx.template("bin/wrapper_strip", ctx.attr.template_bin_strip, {"%{strip}": binaries["strip"]})

"""
Linux specific implementation of the toolchain
"""
_toolchain_maker_linux = repository_rule(
    implementation = _impl,
    attrs = {
        "default": attr.bool(default = False, doc = "Make this compiler the default one, it will be automatically selected when no compiler is given."),
        "cpu": attr.string(),
        "compiler": attr.string(),
        # Compatibility
        "exec_compatible_with": attr.string_list(),
        "target_compatible_with": attr.string_list(),
        # Filegroup for the toolchain
        "ar_files": attr.string_list(mandatory = True),
        "as_files": attr.string_list(mandatory = True),
        "compiler_files": attr.string_list(mandatory = True),
        "linker_files": attr.string_list(mandatory = True),
        "objcopy_files": attr.string_list(mandatory = True),
        "strip_files": attr.string_list(mandatory = True),
        "dynamic_libraries_files": attr.string_list(mandatory = True),
        "static_libraries_files": attr.string_list(mandatory = True),
        # Run-time libraries
        "builtin_include_directories": attr.string_list(),
        "system_directories": attr.string_list(),
        "linker_dirs": attr.string_list(),
        # Flags
        "link_flags": attr.string_list(),
        "compile_flags": attr.string_list(),
        "compile_dev_flags": attr.string_list(),
        "compile_prod_flags": attr.string_list(),
        # Coverage
        "coverage_compile_flags": attr.string_list(),
        "coverage_link_flags": attr.string_list(),
        # Alias
        "alias": attr.string_dict(),
        # Tools
        "binaries": attr.string_dict(),
        # Tools wrapper template
        "template_bin_ar": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_ar"),
        "template_bin_as": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_as"),
        "template_bin_cc": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_cc"),
        "template_bin_cpp": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_cpp"),
        "template_bin_ld": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_ld"),
        "template_bin_cov": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_cov"),
        "template_bin_nm": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_nm"),
        "template_bin_objdump": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_objdump"),
        "template_bin_objcopy": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_objcopy"),
        "template_bin_strip": attr.label(default = "//tools/bazel_build/toolchains/cc:template/bin/wrapper_strip"),
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
 - A compiler target: "@<id>//:compiler"
 - A platform: "@<id>//:platform"
 - An execution platform: "@<id>//:execution_platform"
 - A config setting: "@<id>//:target"
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
            "template_bin_cc": "//tools/bazel_build/toolchains/cc:template/bin/wrapper_cc_start_end_group",
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

    native.register_toolchains(
        "@{}//:toolchain".format(name),
        "@{}//:binary_toolchain".format(name),
    )

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
