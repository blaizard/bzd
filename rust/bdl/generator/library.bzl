"""Rust code generator for bdl library."""

load("@bzd_bdl//:providers.bzl", "BdlInfo")
load("@rules_cc//cc:defs.bzl", "CcInfo")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")
load("@rules_rust//rust:defs.bzl", "rust_common", "rust_library")
load("//rust/bdl/generator:crate_info.bzl", "merge_crate_info")

def label_to_crate_name(label):
    """Convert a bazel label into a crate name.

    The crate name is derived from the full label, so the package is joined
    with the target name to avoid collisions between packages.

    Args:
        label: The label to be converted.

    Returns:
        A sanitized and safe to use crate name.
    """

    if type(label) == "string":
        label = Label(label)

    name = "{}.{}".format(label.package, label.name)
    sanitized_name = "".join([c if c.isalnum() else "_" for c in name.elems()])

    return sanitized_name

def _rust_generate_headers_impl(ctx):
    info = ctx.attr.bdl[BdlInfo]

    args = ctx.actions.args()
    args.add_all(info.direct, before_each = "--bdl")
    for dep in ctx.attr.deps:
        if rust_common.crate_info in dep:
            args.add("--import", dep[rust_common.crate_info].name)
    for dep in ctx.attr.implementation_deps:
        if rust_common.crate_info in dep:
            args.add("--include", dep[rust_common.crate_info].name)
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
        "implementation_deps": attr.label_list(
            doc = "List of language specific implementation dependencies.",
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

def _rust_deps_impl(ctx):
    return [
        DefaultInfo(),
        merge_crate_info(ctx.attr.deps),
    ]

_rust_deps = rule(
    implementation = _rust_deps_impl,
    doc = """Group the Rust dependencies of a bdl library into a single crate group.""",
    attrs = {
        "deps": attr.label_list(doc = "List of bdl dependencies."),
    },
)

def _link_deps_impl(ctx):
    return [
        DefaultInfo(),
        cc_common.merge_cc_infos(
            direct_cc_infos = [
                dep[CcInfo]
                for dep in ctx.attr.deps
                if CcInfo in dep and rust_common.crate_info not in dep and rust_common.crate_group_info not in dep
            ],
        ),
    ]

_link_deps = rule(
    implementation = _link_deps_impl,
    doc = """Merge the native dependencies of a bdl library for manual FFI linkage.""",
    attrs = {
        "deps": attr.label_list(doc = "List of bdl dependencies."),
    },
)

def _generator_rust_library_impl(
        name,
        visibility,
        bdl,
        srcs,  # @unused
        deps,
        implementation_deps,
        **kwargs):
    _rust_generate_headers(
        name = "{}.generate".format(name),
        output = "{}.rs".format(name),
        bdl = bdl,
        deps = deps,
        implementation_deps = implementation_deps,
    )

    _rust_deps(
        name = "{}.deps".format(name),
        deps = deps,
    )

    _link_deps(
        name = "{}.link_deps".format(name),
        deps = deps,
    )

    rust_library(
        name = name,
        crate_name = label_to_crate_name("//{}:{}".format(native.package_name(), name.removesuffix(".rust"))),
        srcs = [
            "{}.rs".format(name),
        ],
        visibility = visibility,
        deps = [
            "{}.deps".format(name),
        ] + implementation_deps + [
            Label("//rust/bdl/generator/impl/adapter:component"),
            Label("//rust/bzd:bzd"),
        ],
        link_deps = [
            "{}.link_deps".format(name),
        ],
        **kwargs
    )

generator_rust_library = macro(
    doc = "Convert a bdl library into a Rust library.",
    implementation = _generator_rust_library_impl,
    inherit_attrs = "common",
    attrs = {
        "bdl": attr.label(mandatory = True, doc = "The bdl target to be converted to a C++ library."),
        "deps": attr.label_list(doc = "The dependencies from the bdl file."),
        "implementation_deps": attr.label_list(doc = "The language specific implementation dependencies."),
        "srcs": attr.label_list(doc = "The bdl source files.", configurable = False),
    },
)
