"""C++ extension for bdl rules."""

load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@bzd_toolchain_cc//cc:defs.bzl", "cc_compile", "cc_link")

def _get_cc_public_header(target):
    """Get all the direct public headers from a target."""

    if CcInfo not in target:
        return []
    return target[CcInfo].compilation_context.direct_public_headers

def _library_providers(ctx, generated):
    return [cc_compile(ctx = ctx, hdrs = generated, deps = ctx.attr._deps_cc + ctx.attr.deps)]

def _library_data(deps):
    return {"includes": {"": [f.short_path for dep in deps for f in _get_cc_public_header(dep)]}}

def _aspect_files(target):
    return _get_cc_public_header(target)

def _composition_data(info, info_per_target):
    def _info_to_includes(infos):
        return [hdr.short_path for hdr in depset(transitive = [info["hdrs"] for info in infos]).to_list()]

    return {"includes": {
        "all": _info_to_includes(info),
    } | {target: _info_to_includes(info) for target, info in info_per_target.items()}}

def _composition_providers(_ctx, output, deps):
    return {"deps": deps, "srcs": [output]}

def _bdl_binary_build(ctx, name, binary_file):
    """Prepare the binary for the execution stage.

    Args:
        ctx: The context of the action.
        name: The name of the output.
        binary_file: The binary of language-specific rule.

    Returns:
        A list of binaries and metadatas
    """

    binary_toolchain = ctx.toolchains["@bzd_toolchain_cc//binary:toolchain_type"].info
    binaries = [binary_file]
    metadatas = []

    # Run the build steps
    for index, build in enumerate(binary_toolchain.build):
        build_binary_file = ctx.actions.declare_file("{}.build.{}".format(name, index))
        ctx.actions.run(
            inputs = binaries,
            outputs = [build_binary_file],
            tools = build.data_runfiles.files,
            arguments = [binary.path for binary in binaries] + [build_binary_file.path],
            executable = build.files_to_run,
        )
        binaries.append(build_binary_file)

    # Run the metadata steps
    for index, metadata in enumerate(binary_toolchain.metadata):
        metadata_file = ctx.actions.declare_file("{}.metadata.{}".format(name, index))
        ctx.actions.run(
            inputs = binaries,
            outputs = [metadata_file],
            tools = metadata.default_runfiles.files,
            arguments = [b.path for b in binaries] + [metadata_file.path],
            executable = metadata.files_to_run,
        )
        metadatas.append(metadata_file)

    return binaries, metadatas

def _bdl_binary_execution(ctx, binaries):
    """Build the executor provider."""

    binary_toolchain = ctx.toolchains["@bzd_toolchain_cc//binary:toolchain_type"].info

    # Identify the executor
    executor = ctx.attr._executor[BuildSettingInfo].value
    executors_mapping = {value: key for key, values in binary_toolchain.executors.items() for value in values.split(",")}
    if executor not in executors_mapping:
        fail("This platform does not support the '{}' executor, only the following are supported: {}.".format(executor, executors_mapping.keys()))

    args = []
    if ctx.attr._debug[BuildSettingInfo].value:
        args.append("--debug")
    args += [binary.short_path for binary in binaries]

    return sh_binary_wrapper_impl(
        ctx = ctx,
        binary = executors_mapping[executor],
        output = ctx.outputs.executable,
        data = binaries,
        command = "{{binary}} {} $@".format(" ".join(args)),
    ), []

def _binary_build(ctx, name, provider):
    binary_file, metadata_files = cc_link(ctx, name = name, srcs = provider["srcs"], deps = provider["deps"], map_analyzer = ctx.attr._map_analyzer_script)

    # Build the binaries.
    binaries, build_metadata = _bdl_binary_build(ctx = ctx, name = name, binary_file = binary_file)

    # Add the execution wrapper.
    default_info, execution_metadata = _bdl_binary_execution(ctx = ctx, binaries = binaries)

    return [default_info, coverage_common.instrumented_files_info(
        ctx,
        dependency_attributes = ["deps"],
    )], ctx.files._metadata_cc + metadata_files + build_metadata + execution_metadata

extension = {
    "cc": {
        "aspect_files": {
            "hdrs": _aspect_files,
        },
        "binary": {
            "build": _binary_build,
            "metadata": [
                Label("//tools/bazel_build/rules/assets/cc:metadata_json"),
            ],
        },
        "composition": {
            "data": _composition_data,
            "deps": [
                Label("//tools/bdl/generators/cc/adapter:context"),
            ],
            "output": "{name}.composition.{target}.cc",
            "providers": _composition_providers,
        },
        "display": "C++",
        "library": {
            "data": _library_data,
            "deps": [
                Label("//tools/bdl/generators/cc/adapter:types"),
            ],
            "outputs": ["{name}.hh"],
            "providers": _library_providers,
        },
    },
}
