load("//tools/bazel_build:binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel_build/rules:package.bzl", "BzdPackageFragment", "BzdPackageMetadataFragment")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_composition")
load("@bazel_skylib//lib:sets.bzl", "sets")
load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//tools/bazel_build/rules/assets/cc:defs.bzl", "cc_compile", "cc_link")
load("@rules_cc//cc:defs.bzl", "cc_test")

def _cc_binary(ctx, binary_file):
    """
    Prepare the binary for the execution stage.

    Returns:
        A tuple containing the DefaultInfo provider and the metadata.
    """

    binary_toolchain = ctx.toolchains["//tools/bazel_build/toolchains/binary:toolchain_type"].info

    # Application binary build stage

    binaries = [binary_file]

    # Run the build steps
    for index, build in enumerate(binary_toolchain.build):
        build_binary_file = ctx.actions.declare_file("{}.build.{}".format(ctx.attr.name, index))
        ctx.actions.run(
            inputs = [binary_file],
            outputs = [build_binary_file],
            tools = build.data_runfiles.files,
            arguments = [binary_file.path, build_binary_file.path],
            executable = build.files_to_run,
        )
        binaries.append(build_binary_file)

    # Application metadata phase

    #metadata_list = binary_toolchain.metadata if binary_toolchain.metadata else [ctx.attr._metadata_script]

    """
    for index, metadata in enumerate(metadata_list):
        is_executable = bool(OutputGroupInfo in metadata)
        if is_executable:
            metadata_file = ctx.actions.declare_file(".bzd/{}.metadata.{}".format(ctx.attr.name, index))
            ctx.actions.run(
                inputs = [executable, binary_final],
                outputs = [metadata_file],
                progress_message = "Generating metadata for {}".format(ctx.attr.name),
                arguments = [executable.path, binary_stripped.path, binary_final.path, metadata_file.path],
                tools = metadata.data_runfiles.files,
                executable = metadata.files_to_run,
            )
            metadata_manifests.append(metadata_file)
        else:
            metadata_manifests.append(metadata.files.to_list()[0])
    """

    # Application execution phase

    # Identify the executor
    executor = ctx.attr._executor[BuildSettingInfo].value
    executors_mapping = {value: key for key, values in binary_toolchain.executors.items() for value in values.split(",")}

    if executor not in executors_mapping:
        fail("This platform does not support the '{}' executor, only the followings are supported: {}.".format(executor, executors_mapping.keys()))

    args = []
    if ctx.attr._debug[BuildSettingInfo].value:
        args.append("--debug")
    args += ["\"{}\"".format(binary.short_path) for binary in binaries]

    default_info = sh_binary_wrapper_impl(
        ctx = ctx,
        binary = executors_mapping[executor],
        output = ctx.outputs.executable,
        extra_runfiles = binaries,
        command = "{{binary}} {} $@".format(" ".join(args)),
    )

    return default_info, []

def _bzd_cc_generic_impl(ctx):
    # Link the CcInfo providers and generate metadata.
    binary_file, link_metadata_files = cc_link(ctx = ctx, srcs = ctx.files.srcs, deps = ctx.attr.deps, map_analyzer = ctx.attr._map_analyzer_script)

    # Make the executor from the binary toolchain
    default_info, binary_metadata_files = _cc_binary(ctx, binary_file)

    # Manifests to be exported.
    manifests = ctx.files._default_metadata_files + binary_metadata_files + link_metadata_files

    return [
        default_info,
        BzdPackageFragment(
            files = [binary_file],
        ),
        BzdPackageMetadataFragment(
            manifests = manifests,
        ),
        OutputGroupInfo(metadata = manifests),
        coverage_common.instrumented_files_info(
            ctx,
            source_attributes = ["srcs"],
            dependency_attributes = ["deps"],
        ),
    ]

def _bzd_cc_generic(is_test):
    return rule(
        implementation = _bzd_cc_generic_impl,
        attrs = {
            "deps": attr.label_list(
                doc = "C++ dependencies",
                providers = [CcInfo],
            ),
            "srcs": attr.label_list(
                doc = "C++ source files",
                allow_files = True,
            ),
            "_map_analyzer_script": attr.label(
                executable = True,
                cfg = "exec",
                default = Label("//tools/bazel_build/rules/assets/cc/map_analyzer"),
            ),
            "_default_metadata_files": attr.label_list(
                default = [
                    Label("//tools/bazel_build/rules/assets/cc:metadata_json"),
                ],
                allow_files = True,
            ),
            "_is_test": attr.bool(
                default = is_test,
            ),
            "_executor": attr.label(
                default = "//tools/bazel_build/settings/executor",
            ),
            "_debug": attr.label(
                default = "//tools/bazel_build/settings/debug",
            ),
            "_lcov_merger": attr.label(
                cfg = "exec",
                executable = True,
                default = Label("@bazel_tools//tools/test:lcov_merger"),
            ),
            "_cc_toolchain": attr.label(default = Label("@rules_cc//cc:current_cc_toolchain")),
        },
        executable = True,
        test = is_test,
        toolchains = [
            "@rules_cc//cc:toolchain_type",
            "//tools/bazel_build/toolchains/binary:toolchain_type",
        ],
        fragments = ["cpp"],
    )

_bzd_cc_binary = _bzd_cc_generic(is_test = False)
_bzd_cc_test = _bzd_cc_generic(is_test = True)

def bzd_cc_binary(name, tags = [], srcs = [], deps = [], **kwags):
    """
    Rule to define a bzd C++ binary.
    """
    bdl_composition(
        name = name + ".composition",
        tags = tags + ["cc"],
        deps = deps + ["//cc/bzd/platform"],
    )
    _bzd_cc_binary(
        name = name,
        tags = tags + ["cc"],
        srcs = srcs,
        deps = [
            name + ".composition",
        ],
        **kwags
    )

def bzd_cc_test(name, tags = [], srcs = [], deps = [], **kwags):
    """
    Rule to define a bzd C++ test.
    """
    bdl_composition(
        name = name + ".composition",
        tags = tags + ["cc"],
        deps = deps + ["//cc/bzd/platform"],
        testonly = True,
    )
    _bzd_cc_test(
        name = name,
        tags = tags + ["cc"],
        srcs = srcs,
        deps = [
            name + ".composition",
        ],
        **kwags
    )

    # Use a specific rule for coverage, I am not able to make _bzd_cc_test
    # compatible with coverage, to be investigated.
    cc_test(
        name = "{}.coverage".format(name),
        tags = tags + ["cc-coverage"],
        srcs = srcs,
        deps = [
            name + ".composition",
        ],
        **kwags
    )

def _bzd_cc_library_impl(ctx):
    # Build the list of public headers
    hdrs = sets.make(ctx.files.hdrs)
    for dep in ctx.attr.deps:
        for f in dep[CcInfo].compilation_context.direct_public_headers:
            sets.insert(hdrs, f)

    # Build the public header file
    hdrs_file = ctx.actions.declare_file("{}.hh".format(ctx.attr.name))
    ctx.actions.write(
        output = hdrs_file,
        content = "#pragma once\n\n{content}".format(content = "\n".join(["#include \"{}\"".format(f.path) for f in sets.to_list(hdrs)])),
    )

    info_provider = cc_compile(
        ctx = ctx,
        hdrs = ctx.files.hdrs + [hdrs_file],
        srcs = ctx.files.srcs,
        deps = ctx.attr.deps,
    )

    return info_provider

_bzd_cc_library = rule(
    doc = """
    Provide a replacement to cc_library with additional functionalities:
    - Generates a header file containing all hdrs and the ones from the direct deps.
    """,
    implementation = _bzd_cc_library_impl,
    attrs = {
        "deps": attr.label_list(
            doc = "C++ dependencies",
            providers = [CcInfo],
        ),
        "hdrs": attr.label_list(
            doc = "C++ header files",
            allow_files = True,
        ),
        "srcs": attr.label_list(
            doc = "C++ source files",
            allow_files = True,
        ),
        "_cc_toolchain": attr.label(default = Label("@rules_cc//cc:current_cc_toolchain")),
    },
    toolchains = [
        "@rules_cc//cc:toolchain_type",
        "//tools/bazel_build/toolchains/binary:toolchain_type",
    ],
    fragments = ["cpp"],
)

def bzd_cc_library(tags = [], **kwags):
    """
    Rule to define a bzd C++ library.
    """
    _bzd_cc_library(
        tags = tags + ["cc"],
        **kwags
    )
