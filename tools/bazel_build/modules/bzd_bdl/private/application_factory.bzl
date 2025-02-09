"""BDL application rule factory."""

load("//private:common.bzl", "precompile_bdl")

visibility(["//..."])

def bdl_application_factory(implementation):
    def _bdl_application_factory_impl(ctx):
        bdl = ctx.actions.declare_file(ctx.label.name + ".bdl")
        namespace = ".".join(ctx.label.package.split("/") + [ctx.label.name])
        providers = implementation(ctx, ctx.attr.target, bdl, namespace)

        bdl_provider, _ = precompile_bdl(ctx, srcs = [bdl], deps = [])

        return [bdl_provider] + providers

    return rule(
        implementation = _bdl_application_factory_impl,
        doc = "Wrapper for an application and add bdl information.",
        attrs = {
            "target": attr.label(
                mandatory = True,
                doc = "The target associated with this application.",
            ),
            "_bdl": attr.label(
                default = Label("//bdl"),
                cfg = "exec",
                executable = True,
            ),
        },
    )
