"""Rust code generator for bdl library."""

load("@bzd_bdl//:providers.bzl", "BdlInfo")
load("@rules_rust//rust:defs.bzl", "rust_library")

def label_to_crate_name(label):
    """Convert a bazel label into a crate name.

    Args:
        label: The label to be converted.

    Returns:
        A sanitized and safe to use crate name.
    """

    if type(label) == "string":
        label = Label(label)

    name = label.name
    sanitized_name = "".join([c if c.isalnum() else "_" for c in name.elems()])

    return sanitized_name

def _rust_generate_headers_impl(ctx):
    info = ctx.attr.bdl[BdlInfo]

    args = ctx.actions.args()
    args.add_all(info.direct, before_each = "--bdl")
    args.add("--output", ctx.outputs.output)

    ctx.actions.run(
        inputs = info.files,
        outputs = [ctx.outputs.output],
        progress_message = "Generating Rust from BDL {}".format(ctx.attr.bdl),
        arguments = [args],
        executable = ctx.executable._tool,
    )

    return [DefaultInfo(files = depset([ctx.outputs.output]))]

_rust_generate_headers = rule(
    implementation = _rust_generate_headers_impl,
    doc = """Generate Rust files from a .bdl file.""",
    attrs = {
        "bdl": attr.label(
            mandatory = True,
            providers = [BdlInfo],
            doc = "The bdl library.",
        ),
        "deps": attr.label_list(
            doc = "List of bdl dependencies.",
        ),
        "output": attr.output(
            mandatory = True,
            doc = "The generated rust file.",
        ),
        "_tool": attr.label(
            default = Label("//rust/bdl/generator:library"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _generator_rust_library_impl(
        name,
        visibility,
        bdl,
        srcs,  # @unused
        deps,
        **kwargs):
    _rust_generate_headers(
        name = "{}.generate".format(name),
        output = "{}.rs".format(name),
        bdl = bdl,
        deps = deps,
    )

    rust_library(
        name = name,
        # TODO: implement proper label_to_crate_name(...).
        # It should be the full label, such as hello_interface for //hello:interface.
        crate_name = label_to_crate_name(name.removesuffix(".rust")),
        srcs = [
            "{}.rs".format(name),
        ],
        visibility = visibility,
        deps = deps + ["//rust/bzd:bzd"],
        **kwargs
    )

generator_rust_library = macro(
    doc = "Convert a bdl library into a Rust library.",
    implementation = _generator_rust_library_impl,
    inherit_attrs = "common",
    attrs = {
        "bdl": attr.label(mandatory = True, doc = "The bdl target to be converted to a C++ library."),
        "deps": attr.label_list(doc = "The dependencies from the bdl file."),
        "srcs": attr.label_list(doc = "The bdl source files.", configurable = False),
    },
)
