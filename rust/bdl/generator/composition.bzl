"""Bdl composition to Rust."""

load("@bzd_bdl//:providers.bzl", "BdlCompositionInfo", "BdlTargetInfo")
load("@rules_rust//rust:defs.bzl", "rust_common", "rust_library")
load("//rust:defs.bzl", "rust_binary")

def _merge_crate_info(deps):
    """Merge rust crate providers into a single one."""

    dep_infos = []
    dep_transitive_infos = []

    for dep in deps:
        if rust_common.crate_info in dep:
            dep_infos.append(
                rust_common.dep_variant_info(
                    crate_info = dep[rust_common.crate_info],
                    dep_info = dep[rust_common.dep_info],
                    cc_info = None,
                    build_info = None,
                ),
            )

        if rust_common.crate_group_info in dep:
            dep_transitive_infos.append(
                dep[rust_common.crate_group_info].dep_variant_infos,
            )

    return rust_common.crate_group_info(
        dep_variant_infos = depset(dep_infos, transitive = dep_transitive_infos),
    )

def _rust_generate_main_impl(ctx):
    target_info = ctx.attr.target[BdlTargetInfo]
    if target_info.language == "rust":
        main = target_info.data["main"][0][DefaultInfo].files.to_list()[0]
        crate_group_info = _merge_crate_info(target_info.data["deps"])
    else:
        main = ctx.actions.declare_file("{}.main.rs".format(ctx.label.name))
        ctx.actions.write(
            output = main,
            content = "",
        )
        crate_group_info = _merge_crate_info([])
    return [DefaultInfo(files = depset([main])), crate_group_info]

_rust_generate_main = rule(
    implementation = _rust_generate_main_impl,
    doc = """Generate main.rs.""",
    attrs = {
        "target": attr.label(
            mandatory = True,
            providers = [BdlTargetInfo],
            doc = "The target to be used.",
        ),
    },
)

def _rust_generate_composition_impl(ctx):
    info = ctx.attr.composition[BdlCompositionInfo]
    all_deps = ctx.attr.deps + info.deps[ctx.attr.target_name]
    crate_group_info = _merge_crate_info(all_deps)

    ctx.actions.write(
        output = ctx.outputs.output,
        content = """#![no_std]

pub async fn executor() -> i32 {
    42
}
""",
    )

    return [DefaultInfo(files = depset([ctx.outputs.output])), crate_group_info]

_rust_generate_composition = rule(
    implementation = _rust_generate_composition_impl,
    doc = """Generate Rust composition.""",
    attrs = {
        "composition": attr.label(
            mandatory = True,
            providers = [BdlCompositionInfo],
            doc = "The system composition.",
        ),
        "deps": attr.label_list(
            doc = "List of bdl dependencies.",
        ),
        "output": attr.output(
            mandatory = True,
            doc = "The generated rust composition file.",
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

def _generator_rust_composition_impl(name, visibility, target_name, target, composition, deps, testonly, **kwargs):
    _rust_generate_composition(
        name = "{}.composition".format(name),
        output = "{}.composition.rs".format(name),
        target_name = target_name,
        target = target,
        composition = composition,
        deps = deps,
        testonly = testonly,
        tags = ["manual"],
    )

    rust_library(
        name = "{}.library".format(name),
        crate_name = "composition",
        testonly = testonly,
        srcs = [
            "{}.composition.rs".format(name),
        ],
        deps = [
            "{}.composition".format(name),
        ],
        tags = ["manual"],
    )

    _rust_generate_main(
        name = "{}.main".format(name),
        testonly = testonly,
        target = target,
        tags = ["manual"],
    )

    rust_binary(
        name = name,
        crate_name = "binary",
        srcs = [
            "{}.main".format(name),
        ],
        deps = [
            "{}.library".format(name),
            "{}.main".format(name),
        ],
        visibility = visibility,
        testonly = testonly,
        **kwargs
    )

generator_rust_composition = macro(
    doc = "Convert a bdl composition into a Rust binary.",
    implementation = _generator_rust_composition_impl,
    inherit_attrs = "common",
    attrs = {
        "composition": attr.label(mandatory = True, providers = [BdlCompositionInfo], doc = "Information about the system composition."),
        "deps": attr.label_list(doc = "The common dependencies for the system."),
        "target": attr.label(mandatory = True, providers = [BdlTargetInfo], doc = "The target."),
        "target_name": attr.string(mandatory = True, doc = "The name of the target to process."),
    },
)
