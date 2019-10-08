def _impl(ctx):
    build_substitutions = {
        "%{cpu}": ctx.attr.cpu,
        "%{compiler}": ctx.attr.compiler,
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
            ["'-idirafter', '{}',".format(t) for t in ctx.attr.system_directories] +
            ["'-I', '{}',".format(t) for t in ctx.attr.system_directories] +
            ["'{}',".format(t) for t in ctx.attr.compile_flags],
        ),
        "%{link_flags}": "\n".join(
            ["'-L{}',".format(t) for t in ctx.attr.linker_dirs] +
            ["'{}',".format(t) for t in ctx.attr.link_flags],
        ),
        "%{exec_compatible_with}": "\n".join(['"{}",'.format(t) for t in ctx.attr.exec_compatible_with]),
        "%{target_compatible_with}": "\n".join(['"{}",'.format(t) for t in ctx.attr.target_compatible_with]),
    }

    # Set the toolchain BUILD
    ctx.template("BUILD", Label("//tools/bazel:toolchains/template/BUILD.tpl"), build_substitutions)

    # Set the binary wrappers
    ctx.template("bin/wrapper-ar", Label("//tools/bazel:toolchains/template/bin/wrapper-ar"), {"%{ar}": ctx.attr.bin_ar})
    ctx.template("bin/wrapper-as", Label("//tools/bazel:toolchains/template/bin/wrapper-as"), {"%{as}": ctx.attr.bin_as})
    ctx.template("bin/wrapper-cc", Label("//tools/bazel:toolchains/template/bin/wrapper-cc"), {"%{cc}": ctx.attr.bin_cc})
    ctx.template("bin/wrapper-cpp", Label("//tools/bazel:toolchains/template/bin/wrapper-cpp"), {"%{cpp}": ctx.attr.bin_cpp})
    ctx.template("bin/wrapper-ld", Label("//tools/bazel:toolchains/template/bin/wrapper-ld"), {"%{ld}": ctx.attr.bin_ld})
    ctx.template("bin/wrapper-gcov", Label("//tools/bazel:toolchains/template/bin/wrapper-gcov"), {"%{gcov}": ctx.attr.bin_gcov})
    ctx.template("bin/wrapper-nm", Label("//tools/bazel:toolchains/template/bin/wrapper-nm"), {"%{nm}": ctx.attr.bin_nm})
    ctx.template("bin/wrapper-objdump", Label("//tools/bazel:toolchains/template/bin/wrapper-objdump"), {"%{objdump}": ctx.attr.bin_objdump})
    ctx.template("bin/wrapper-objcopy", Label("//tools/bazel:toolchains/template/bin/wrapper-objcopy"), {"%{objcopy}": ctx.attr.bin_objcopy})
    ctx.template("bin/wrapper-strip", Label("//tools/bazel:toolchains/template/bin/wrapper-strip"), {"%{strip}": ctx.attr.bin_strip})

toolchain_maker = repository_rule(
    implementation = _impl,
    attrs = {
        "cpu": attr.string(),
        "compiler": attr.string(),
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
        # Tools
        "bin_ar": attr.string(default = "/usr/bin/ar"),
        "bin_as": attr.string(),
        "bin_cc": attr.string(),
        "bin_cpp": attr.string(),
        "bin_ld": attr.string(default = "/usr/bin/ld"),
        "bin_gcov": attr.string(default = "/usr/bin/gcov"),
        "bin_nm": attr.string(default = "/usr/bin/nm"),
        "bin_objdump": attr.string(default = "/usr/bin/objdump"),
        "bin_objcopy": attr.string(default = "/usr/bin/objcopy"),
        "bin_strip": attr.string(default = "/usr/bin/strip"),
    },
)
