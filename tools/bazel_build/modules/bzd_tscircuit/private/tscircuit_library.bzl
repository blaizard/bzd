"""Rules for tscircuit libraries."""

load("//private:provider.bzl", "BzdTscircuitInfo")

def _bzd_tscircuit_library_impl(ctx):
    files = depset(
        ctx.files.srcs,
        transitive = [dep[BzdTscircuitInfo].files for dep in ctx.attr.deps],
    )

    return [
        DefaultInfo(files = files),
        BzdTscircuitInfo(files = files),
    ]

bzd_tscircuit_library = rule(
    doc = """Gather the source files of a tscircuit library.""",
    implementation = _bzd_tscircuit_library_impl,
    attrs = {
        "deps": attr.label_list(
            doc = "Libraries this library depends on.",
            providers = [BzdTscircuitInfo],
        ),
        "srcs": attr.label_list(
            allow_files = True,
            doc = "Source files of the library.",
        ),
    },
    provides = [BzdTscircuitInfo],
)
