AppInfo = provider(
    doc = "blah blah.",
    fields = ["prepare", "info", "execute"],
)

def _app_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        app = AppInfo(
            prepare = ctx.attr.prepare,
            info = ctx.attr.info,
            execute = ctx.attr.execute,
        ),
    )
    return [toolchain_info]

"""
Defines an application toolchain.
An application toolchain extends a compiler toolchain but added functionalities
like execute, profile...
"""
app_toolchain = rule(
    implementation = _app_toolchain_impl,
    attrs = {
        "prepare": attr.label(
            executable = True,
            cfg = "host",
        ),
        "info": attr.string(),
        "execute": attr.label(
            executable = True,
            cfg = "host",
        ),
    },
)

def _impl(ctx):
    # Absolute path of the external directory (used for linking librairies for example)
    absolute_external = ctx.execute(["pwd"], working_directory = "..").stdout.rstrip()

    alias_template = "alias(name = \"{}\", actual = \"{}\")"

    # Build the application toolchain arguments
    app_kwargs = []
    if ctx.attr.app_prepare:
        app_kwargs.append("prepare = \"{}\",".format(ctx.attr.app_prepare))
    if ctx.attr.app_info:
        app_kwargs.append("info = \"{}\",".format(ctx.attr.app_info))
    if ctx.attr.app_execute:
        app_kwargs.append("execute = \"{}\",".format(ctx.attr.app_execute))

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
        "%{system_directories}": "\n".join(["'{}',".format(t) for t in ctx.attr.builtin_include_directories]),
        "%{compile_flags}": "\n".join(
            ["'-isystem', '{}',".format(t) for t in ctx.attr.system_directories] +
            ["'{}',".format(t) for t in ctx.attr.compile_flags],
        ),
        "%{link_flags}": "\n".join(
            ["'-L{}',".format(t) for t in ctx.attr.linker_dirs] +
            ["'{}',".format(t) for t in ctx.attr.link_flags],
        ),
        "%{exec_compatible_with}": "\n".join(['"{}",'.format(t) for t in ctx.attr.exec_compatible_with]),
        "%{target_compatible_with}": "\n".join(['"{}",'.format(t) for t in ctx.attr.target_compatible_with]),
        "%{alias}": "\n".join([alias_template.format(k, v) for k, v in ctx.attr.alias.items()]),
        "%{app_kwargs}": "\n".join(app_kwargs),
    }

    # Apply substitutions
    build_substitutions = {k: v.replace("%{absolute_external}", absolute_external) for k, v in build_substitutions.items()}

    # Set the toolchain BUILD
    ctx.template("BUILD", Label("//tools/bazel.build/toolchains/cc:template/BUILD.tpl"), build_substitutions)

    # Set the binary wrappers
    ctx.template("bin/wrapper-ar", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-ar"), {"%{ar}": ctx.attr.bin_ar})
    ctx.template("bin/wrapper-as", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-as"), {"%{as}": ctx.attr.bin_as})
    ctx.template("bin/wrapper-cc", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-cc"), {"%{cc}": ctx.attr.bin_cc})
    ctx.template("bin/wrapper-cpp", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-cpp"), {"%{cpp}": ctx.attr.bin_cpp})
    ctx.template("bin/wrapper-ld", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-ld"), {"%{ld}": ctx.attr.bin_ld})
    ctx.template("bin/wrapper-cov", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-cov"), {"%{cov}": ctx.attr.bin_cov})
    ctx.template("bin/wrapper-nm", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-nm"), {"%{nm}": ctx.attr.bin_nm})
    ctx.template("bin/wrapper-objdump", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-objdump"), {"%{objdump}": ctx.attr.bin_objdump})
    ctx.template("bin/wrapper-objcopy", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-objcopy"), {"%{objcopy}": ctx.attr.bin_objcopy})
    ctx.template("bin/wrapper-strip", Label("//tools/bazel.build/toolchains/cc:template/bin/wrapper-strip"), {"%{strip}": ctx.attr.bin_strip})

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
        "app_info": attr.string(),
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
    if implementation == "linux":
        _toolchain_maker_linux(
            name = name,
            **definition
        )

        native.register_toolchains(
            "@{}//:toolchain".format(name),
            "@{}//:app_toolchain".format(name),
        )

        native.register_execution_platforms(
            "@{}//:host_platform".format(name),
        )

    else:
        fail("Unsupported toolchain type '{}'".format(implementation))

"""
Merge 2 toolchain data entries.
"""

def toolchain_merge(data1, data2):
    for key2, value2 in data2.items():
        if key2 in data1:
            if type(data1[key2]) != type(value2):
                fail("Trying to merge conflicting types for key '{}'.".format(key2))
            if type(value2) == "list":
                data1[key2] += value2
            elif data1[key2] != value2:
                fail("Trying to merge different values for key '{}'.".format(key2))
        else:
            data1[key2] = value2

    return data1
