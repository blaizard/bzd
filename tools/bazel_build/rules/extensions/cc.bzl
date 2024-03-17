"""C++ extension for bdl rules."""

load("@bzd_toolchain_cc//cc:defs.bzl", "cc_compile")

def _library_providers(ctx, generated):
    return [cc_compile(ctx = ctx, hdrs = generated, deps = ctx.attr._deps_cc + ctx.attr.deps)]

extension = {
    "cc": {
        "display": "C++",
        "library_deps": [
            Label("//tools/bdl/generators/cc/adapter:types"),
        ],
        "library_providers": _library_providers,
        "outputs": ["{}.hh"],
    },
}
