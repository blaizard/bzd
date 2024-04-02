"""C++ extension for bdl rules."""

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

def _binary(ctx, name, provider):
    binary_file, metadata_files = cc_link(ctx, name = name, srcs = provider["srcs"], deps = provider["deps"], map_analyzer = ctx.attr._map_analyzer_script)
    metadata = ctx.files._metadata_cc
    extra_providers = [coverage_common.instrumented_files_info(
        ctx,
        dependency_attributes = ["deps"],
    )]
    return binary_file, metadata_files + metadata, extra_providers

extension = {
    "cc": {
        "aspect_files": {
            "hdrs": _aspect_files,
        },
        "binary": _binary,
        "binary_metadata": [
            Label("//tools/bazel_build/rules/assets/cc:metadata_json"),
        ],
        "composition_data": _composition_data,
        "composition_deps": [
            Label("//tools/bdl/generators/cc/adapter:context"),
        ],
        "composition_output": "{name}.composition.{target}.cc",
        "composition_providers": _composition_providers,
        "display": "C++",
        "library_data": _library_data,
        "library_deps": [
            Label("//tools/bdl/generators/cc/adapter:types"),
        ],
        "library_outputs": ["{name}.hh"],
        "library_providers": _library_providers,
    },
}
