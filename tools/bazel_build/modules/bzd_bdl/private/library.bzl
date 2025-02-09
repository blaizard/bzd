"""BDL library rules."""

load("//private:common.bzl", "aspect_bdl_providers", "library_extensions", "make_bdl_arguments", "precompile_bdl")
load("//private:providers.bzl", "BdlInfo", "BdlTagInfo")

visibility(["//..."])

def _bdl_library_impl(ctx):
    # Pre-compile the BDLs into their language agnostics format.
    bdl_provider, metadata = precompile_bdl(
        ctx = ctx,
        srcs = ctx.files.srcs,
        deps = ctx.attr.deps,
    )

    # Generate the data for the data file for the bdl constructor.
    language_specific_data = {}
    for fmt, data in library_extensions.items():
        if "library" in data:
            language_specific_data[fmt] = data["library"]["data"](ctx.attr.deps)

    # Write the language specific data file.
    data_file = ctx.actions.declare_file("{}.data.json".format(ctx.label.name))
    ctx.actions.write(
        output = data_file,
        content = json.encode(language_specific_data),
    )

    # Generate the various providers
    providers = []
    for fmt, data in library_extensions.items():
        if "library" in data:
            generated = []
            for bdl in metadata:
                # Generate the output
                outputs = [ctx.actions.declare_file(output.format(name = bdl["relative_name"])) for output in data["library"]["outputs"]]
                ctx.actions.run(
                    inputs = depset([data_file], transitive = [bdl_provider.files]),
                    outputs = outputs,
                    progress_message = "Generating {} build files from manifest {}".format(data["display"], bdl["input"].short_path),
                    arguments = make_bdl_arguments(
                        ctx = ctx,
                        stage = "generate",
                        search_formats = bdl_provider.search_formats,
                        format = fmt,
                        output = outputs[0].path,
                        data = data_file,
                        args = [bdl["input"].path],
                    ),
                    executable = ctx.attr._bdl.files_to_run,
                )
                generated += outputs

            # Generate the various providers
            providers.extend(data["library"]["providers"](ctx = ctx, generated = generated))

    return [
        bdl_provider,
        BdlTagInfo(),
    ] + providers

bdl_library = rule(
    implementation = _bdl_library_impl,
    doc = """Bzd Description Language generator rule.
    It generates language provider from a .bdl file.
    The files are generated at the same path of the target, with the name of the target appended with a language specific file extension.
    """,
    attrs = {
        "deps": attr.label_list(
            providers = [BdlInfo],
            aspects = [aspect_bdl_providers],
            doc = "List of bdl dependencies. Language specific dependencies will have their public interface included in the generated file.",
        ),
        "srcs": attr.label_list(
            mandatory = True,
            allow_files = [".bdl"],
            doc = "List of Bzd Description Language (bdl) files to be included.",
        ),
        "_bdl": attr.label(
            default = Label("//bdl"),
            cfg = "exec",
            executable = True,
        ),
    } | {("_deps_" + name): (attr.label_list(
        default = data["library"]["deps"],
    )) for name, data in library_extensions.items() if "library" in data},
    toolchains = [
        "@rules_cc//cc:toolchain_type",
    ],
    fragments = ["cpp"],
)
