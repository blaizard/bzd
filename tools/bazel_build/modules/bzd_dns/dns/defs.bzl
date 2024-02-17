"""DNS bazel rules."""

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
            default = Label("@bzd_dns//python/bzd_dns:aggregator"),
            cfg = "exec",
            executable = True,
        ),
    },
)

def _bzd_dns_binary_impl(ctx):
    output = ctx.actions.declare_directory(ctx.label.name + ".sync")

    args = ctx.actions.args()
    args.add("--output", output.path)
    for target, domains in ctx.attr.srcs.items():
        for domain in domains.split(","):
            for f in target[DefaultInfo].files.to_list():
                args.add_all("--input", [domain, f])

    # Generate the file structure under output.
    ctx.actions.run(
        inputs = ctx.files.srcs,
        outputs = [output],
        progress_message = "Preparing DNS zones for {}...".format(ctx.label),
        arguments = [args],
        executable = ctx.executable._build,
    )

    return [DefaultInfo(files = depset([output]))]

bzd_dns_binary = rule(
    implementation = _bzd_dns_binary_impl,
    doc = "Create a DNS binary, used to validate and set DNS records",
    attrs = {
        "config": attr.label(
            doc = "Configuration for the provider.",
            allow_single_file = [".json"],
        ),
        "provider": attr.string(
            mandatory = True,
            values = ["digitalocean"],
            doc = "Supported providers.",
        ),
        "srcs": attr.label_keyed_string_dict(
            mandatory = True,
            doc = "Zones to domain mapping.",
            allow_files = [".json"],
        ),
        "_build": attr.label(
            default = Label("@bzd_dns//python/bzd_dns:build"),
            cfg = "exec",
            executable = True,
        ),
    },
    #executable = True
)
