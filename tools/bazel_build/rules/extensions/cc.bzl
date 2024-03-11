"""C++ extension for bdl rules."""

load("@bzd_toolchain_cc//cc:defs.bzl", "cc_compile")

def _make_library_provider(ctx, generated):
    return cc_compile(ctx = ctx, hdrs = generated, deps = ctx.attr._deps_cc + ctx.attr.deps)

extension = {
    "cc": {
        "display": "C++",
        "make_library_provider": _make_library_provider,
        "outputs": ["{}.hh"],
    },
}
