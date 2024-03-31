"""C++ extension for bdl rules."""

load("@bzd_toolchain_cc//cc:defs.bzl", "cc_compile")

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

extension = {
    "cc": {
        "aspect_files": {
            "hdrs": _aspect_files,
        },
        "display": "C++",
        "library_data": _library_data,
        "library_deps": [
            Label("//tools/bdl/generators/cc/adapter:types"),
        ],
        "library_outputs": ["{}.hh"],
        "library_providers": _library_providers,
    },
}
