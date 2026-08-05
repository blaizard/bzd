"""Bdl composition to C++."""

load("@bzd_bdl//:providers.bzl", "BdlCompositionInfo", "BdlTargetInfo")
load("@rules_cc//cc:defs.bzl", "CcInfo", "cc_binary")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")

def _get_cc_public_header(target):
    """Get all the direct public headers from a target."""

    if CcInfo not in target:
        return []
    return target[CcInfo].compilation_context.direct_public_headers

def _cc_generate_composition_impl(ctx):
    info = ctx.attr.composition[BdlCompositionInfo]

    output = ctx.actions.declare_file("{}.cc".format(ctx.label.name))
    args = ctx.actions.args()
    args.add("--output", output)
    args.add("--target", ctx.attr.target_name)

    all_deps = ctx.attr.deps + info.deps[ctx.attr.target_name]
    args.add_all([f.short_path for dep in all_deps for f in _get_cc_public_header(dep)], before_each = "--include")

    sources = [source[1] for provider in info.bdls for source in provider.sources.to_list()]
    args.add_all(sources)

    ctx.actions.run(
        inputs = sources,
        outputs = [output],
        progress_message = "Composing C++ from BDL for {} in {}".format(ctx.attr.target_name, ctx.label),
        arguments = [args],
        executable = ctx.executable._tool,
    )

    deps_cc_infos = [dep[CcInfo] for dep in all_deps if CcInfo in dep]
    merged_cc_info = cc_common.merge_cc_infos(
        direct_cc_infos = deps_cc_infos,
    )

    return [DefaultInfo(files = depset([output])), merged_cc_info]

_cc_generate_composition = rule(
    implementation = _cc_generate_composition_impl,
    doc = """Generate C++ composition files for each targets.""",
    attrs = {
        "composition": attr.label(
            mandatory = True,
            providers = [BdlCompositionInfo],
            doc = "The system composition.",
        ),
        "deps": attr.label_list(
            doc = "List of bdl dependencies.",
        ),
        "target": attr.label(
            mandatory = True,
            providers = [BdlTargetInfo],
            doc = "The target to focus on.",
        ),
        "target_name": attr.string(
            mandatory = True,
            doc = "The name of the target in the system.",
        ),
        "_tool": attr.label(
            default = Label("//cc/bdl/generator:composition"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _generator_cc_composition_impl(name, visibility, target_name, target, composition, deps, tags, testonly, **kwargs):
    _cc_generate_composition(
        name = "{}.generate".format(name),
        target_name = target_name,
        target = target,
        composition = composition,
        deps = deps,
        testonly = testonly,
        tags = ["manual", "no-clang-tidy"],
    )

    cc_binary(
        name = name,
        srcs = [
            "{}.generate".format(name),
        ],
        deps = [
            Label("//cc/bdl/generator/impl/adapter:context"),
            "{}.generate".format(name),
        ],
        visibility = visibility,
        tags = ["no-clang-tidy"] + (tags or []),
        testonly = testonly,
        **kwargs
    )

generator_cc_composition = macro(
    doc = "Convert a bdl composition into a C++ library.",
    implementation = _generator_cc_composition_impl,
    inherit_attrs = "common",
    attrs = {
        "composition": attr.label(mandatory = True, providers = [BdlCompositionInfo], doc = "Information about the system composition."),
        "deps": attr.label_list(doc = "The common dependencies for the system."),
        "target": attr.label(mandatory = True, providers = [BdlTargetInfo], doc = "The target."),
        "target_name": attr.string(mandatory = True, doc = "The name of the target to process."),
    },
)
