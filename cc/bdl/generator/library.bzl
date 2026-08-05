"""C++ code generator for bdl library."""

load("@bzd_bdl//:providers.bzl", "BdlInfo")
load("@rules_cc//cc:defs.bzl", "CcInfo", "cc_library")

def _get_cc_public_header(target):
    """Get all the direct public headers from a target."""

    if CcInfo not in target:
        return []
    return target[CcInfo].compilation_context.direct_public_headers

def _cc_generate_headers_impl(ctx):
    info = ctx.attr.bdl[BdlInfo]

    outputs = []
    args = ctx.actions.args()
    for f in info.direct:
        output = ctx.actions.declare_file("{}.hh".format(f.basename.replace(".bdl.o", "")), sibling = f)
        outputs.append(output)
        args.add_all("--bdl", [f, output])
    args.add_all([f.short_path for dep in ctx.attr.deps for f in _get_cc_public_header(dep)], before_each = "--include")

    ctx.actions.run(
        inputs = info.files,
        outputs = outputs,
        progress_message = "Generating C++ from BDL {}".format(ctx.attr.bdl),
        arguments = [args],
        executable = ctx.executable._tool,
    )

    return [DefaultInfo(files = depset(outputs))]

_cc_generate_headers = rule(
    implementation = _cc_generate_headers_impl,
    doc = """Generate C++ header files from a .bdl file.""",
    attrs = {
        "bdl": attr.label(
            mandatory = True,
            providers = [BdlInfo],
            doc = "The bdl library.",
        ),
        "deps": attr.label_list(
            doc = "List of bdl dependencies.",
        ),
        "_tool": attr.label(
            default = Label("//cc/bdl/generator:library"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _generator_cc_library_impl(
        name,
        visibility,
        bdl,
        srcs,  # @unused
        deps,
        tags,
        **kwargs):
    _cc_generate_headers(
        name = "{}.generate".format(name),
        bdl = bdl,
        deps = deps,
    )

    cc_library(
        name = name,
        hdrs = [
            "{}.generate".format(name),
        ],
        deps = deps + [
            Label("//cc/bdl/generator/impl/adapter:types"),
        ],
        visibility = visibility,
        tags = ["no-clang-tidy"] + (tags or []),
        **kwargs
    )

generator_cc_library = macro(
    doc = "Convert a bdl library into a C++ library.",
    implementation = _generator_cc_library_impl,
    attrs = {
        "bdl": attr.label(mandatory = True, doc = "The bdl target to be converted to a C++ library."),
        "deps": attr.label_list(doc = "The dependencies from the bdl file."),
        "srcs": attr.label_list(doc = "The bdl source files.", configurable = False),
        "tags": attr.string_list(doc = "Tags for the generated C++ library.", configurable = False),
    },
)
