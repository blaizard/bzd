"""DNS bazel rules."""

load("@bzd_lib//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def _bzd_dns_zones_impl(ctx):
    output = ctx.outputs.output_json
    if not output:
        output = ctx.actions.declare_file(ctx.label.name + ".json")

    input_files = []
    input_files += ctx.files.srcs

    args = ctx.actions.args()
    args.add("--output", output.path)
    args.add_all(ctx.files.srcs)
    if ctx.file.config:
        args.add("--config", ctx.file.config)
        input_files.append(ctx.file.config)

    # Build the aggregated configuration.
    ctx.actions.run(
        inputs = input_files,
        outputs = [output],
        progress_message = "Aggregating DNS zones for {}...".format(ctx.label),
        arguments = [args],
        executable = ctx.executable._aggregator,
    )

    return [DefaultInfo(files = depset([output]))]

bzd_dns_zones = rule(
    implementation = _bzd_dns_zones_impl,
    doc = "Create a DNS zones",
    attrs = {
        "config": attr.label(
            doc = "Configuration used for substitution.",
            allow_single_file = [".json"],
        ),
        "output_json": attr.output(
            doc = "Create a json zones.",
        ),
        "srcs": attr.label_list(
            mandatory = True,
            doc = "Collection of zones.",
            allow_files = [".json"],
        ),
        "_aggregator": attr.label(
            default = Label("//python/bzd_dns:aggregator"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _bzd_dns_binary_impl(ctx):
    output = ctx.actions.declare_directory(ctx.label.name + ".sync")

    args = ctx.actions.args()
    args.add("--output", output.path)
    args.add("--relative", output.short_path)
    for target, domains in ctx.attr.srcs.items():
        for domain in domains.split(","):
            for f in target[DefaultInfo].files.to_list():
                args.add_all("--input", [domain, f])
    args.add(ctx.file.config)

    # Generate the file structure under output.
    ctx.actions.run(
        inputs = ctx.files.srcs + [ctx.file.config],
        outputs = [output],
        progress_message = "Generating DNS zones for {}...".format(ctx.label),
        arguments = [args],
        executable = ctx.executable._build,
    )

    # Run octodns.
    return sh_binary_wrapper_impl(
        ctx = ctx,
        locations = {
            ctx.attr._octodns: "octodns",
            output: "output",
        },
        output = ctx.outputs.executable,
        command = "{octodns} --config={output}/config.yaml --quiet --doit",
    )

bzd_dns_binary = rule(
    implementation = _bzd_dns_binary_impl,
    doc = "Create a DNS binary, used to validate and set DNS records",
    attrs = {
        "config": attr.label(
            doc = "Configuration for the provider.",
            allow_single_file = [".json"],
            mandatory = True,
        ),
        "srcs": attr.label_keyed_string_dict(
            mandatory = True,
            doc = "Zones to domain mapping.",
            allow_files = [".json"],
        ),
        "_build": attr.label(
            default = Label("//python/bzd_dns:build"),
            cfg = "exec",
            executable = True,
        ),
        "_octodns": attr.label(
            default = Label("//python/bzd_dns:octodns-sync"),
            cfg = "exec",
            executable = True,
        ),
    },
    executable = True,
)
