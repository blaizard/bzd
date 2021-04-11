load("//tools/bazel_build/toolchains/cc:flags.bzl", "COPTS_GCC", "COPTS_GCC_DEV", "COPTS_GCC_PROD", "COPTS_GCC_COVERAGE", "LINKOPTS_GCC", "LINKOPTS_GCC_COVERAGE", "COPTS_CLANG", "COPTS_CLANG_DEV", "COPTS_CLANG_PROD", "COPTS_CLANG_COVERAGE", "LINKOPTS_CLANG", "LINKOPTS_CLANG_COVERAGE")

def _impl(ctx):
    # Absolute path of the external directory (used for linking librairies for example)
    absolute_external = ctx.execute(["pwd"], working_directory = "..").stdout.rstrip()

    alias_template = "alias(name = \"{}\", actual = \"{}\")"

    # Build the binary toolchain arguments
    binary_kwargs = []
    if ctx.attr.app_prepare:
        binary_kwargs.append("prepare = \"{}\",".format(ctx.attr.app_prepare))
    if ctx.attr.app_metadatas:
        binary_kwargs.append("metadatas = [{}],".format(", ".join(["\"{}\"".format(metadata) for metadata in ctx.attr.app_metadatas])))
    if ctx.attr.app_execute:
        binary_kwargs.append("execute = \"{}\",".format(ctx.attr.app_execute))

    exec_properties = {}
    if ctx.attr.docker_image:
        exec_properties["container-image"] = ctx.attr.docker_image

    build_substitutions = {
        "%{cpu}": ctx.attr.cpu,
        "%{compiler}": ctx.attr.compiler,
        "%{platforms}": "\n".join(['"{}",'.format(t) for t in ctx.attr.platforms]),
        "%{host_platforms}": "\n".join(['"{}",'.format(t) for t in ctx.attr.host_platforms]),
        "%{filegroup_dependencies}": "\n".join(
            ['"{}",'.format(t) for t in ctx.attr.filegroup_dependencies] +
            ["'{}',".format(t) for t in ctx.attr.dynamic_runtime_libs] +
            ["'{}',".format(t) for t in ctx.attr.static_runtime_libs],
        ),
        "%{dynamic_runtime_libs}": "\n".join(['"{}",'.format(t) for t in ctx.attr.dynamic_runtime_libs]),
        "%{static_runtime_libs}": "\n".join(['"{}",'.format(t) for t in ctx.attr.static_runtime_libs]),
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
        "%{exec_properties}": "\n".join(['"{}": "{}"'.format(k, v) for k, v in exec_properties.items()]),
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

    # Set the binary wrappers
    ctx.template("bin/wrapper_ar", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_ar"), {"%{ar}": ctx.attr.bin_ar})
    ctx.template("bin/wrapper_as", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_as"), {"%{as}": ctx.attr.bin_as})
    ctx.template("bin/wrapper_cc", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_cc"), {"%{cc}": ctx.attr.bin_cc})
    ctx.template("bin/wrapper_cpp", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_cpp"), {"%{cpp}": ctx.attr.bin_cpp})
    ctx.template("bin/wrapper_ld", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_ld"), {"%{ld}": ctx.attr.bin_ld})
    ctx.template("bin/wrapper_cov", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_cov"), {"%{cov}": ctx.attr.bin_cov})
    ctx.template("bin/wrapper_nm", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_nm"), {"%{nm}": ctx.attr.bin_nm})
    ctx.template("bin/wrapper_objdump", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_objdump"), {"%{objdump}": ctx.attr.bin_objdump})
    ctx.template("bin/wrapper_objcopy", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_objcopy"), {"%{objcopy}": ctx.attr.bin_objcopy})
    ctx.template("bin/wrapper_strip", Label("//tools/bazel_build/toolchains/cc:template/bin/wrapper_strip"), {"%{strip}": ctx.attr.bin_strip})

"""
Linux specific implementation of the toolchain
"""
_toolchain_maker_linux = repository_rule(
    implementation = _impl,
    attrs = {
        "cpu": attr.string(),
        "compiler": attr.string(),
        "platforms": attr.string_list(),
        "host_platforms": attr.string_list(),
        # Docker image
        "docker_image": attr.string(),
        # Compatibility
        "exec_compatible_with": attr.string_list(),
        "target_compatible_with": attr.string_list(),
        # File group dependencies, files ot be added to the dependency list
        "filegroup_dependencies": attr.string_list(),
        # Run-time libraries
        "dynamic_runtime_libs": attr.string_list(),
        "static_runtime_libs": attr.string_list(),
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
        "bin_ar": attr.string(default = "/usr/bin/ar"),
        "bin_as": attr.string(),
        "bin_cc": attr.string(),
        "bin_cpp": attr.string(),
        "bin_ld": attr.string(default = "/usr/bin/ld"),
        "bin_cov": attr.string(default = "/usr/bin/gcov"),
        "bin_nm": attr.string(default = "/usr/bin/nm"),
        "bin_objdump": attr.string(default = "/usr/bin/objdump"),
        "bin_objcopy": attr.string(default = "/usr/bin/objcopy"),
        "bin_strip": attr.string(default = "/usr/bin/strip"),
        # Execution
        "app_prepare": attr.string(),
        "app_metadatas": attr.string_list(),
        "app_execute": attr.string(),
    },
)

"""
Creates a toolchain to be used with bazel.

This rule also creates few important assets:
 - A toolchain: "@<id>//:toolchain"
 - A compiler target: "@<id>//:compiler"
 - A platform: "@<id>//:platform"
 - A host platform: "@<id>//:host_platform"
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

    native.register_execution_platforms(
        "@{}//:host_platform".format(name),
    )

"""
Merge 2 toolchain data entries.
"""

def toolchain_merge(data1, data2):

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
