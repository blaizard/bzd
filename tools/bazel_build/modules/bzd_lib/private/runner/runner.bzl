"""Runner rules."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")

def bzd_runner_factory(is_test, providers = None):
    def _impl(ctx):
        extra_providers = [ctx.attr.binary[provider] for provider in (providers or []) if provider in ctx.attr.binary]
        toolchain = ctx.toolchains[Label("//private/runner:toolchain_type")]
        if not toolchain.runner:
            ctx.actions.symlink(
                output = ctx.outputs.executable,
                is_executable = True,
                target_file = ctx.attr.binary.files_to_run.executable,
            )
            default_info = DefaultInfo(
                executable = ctx.outputs.executable,
                runfiles = ctx.attr.binary.default_runfiles,
            )
        else:
            default_info = sh_binary_wrapper_impl(
                ctx = ctx,
                locations = {
                    toolchain.runner: "runner",
                    ctx.attr.binary: "binary",
                },
                output = ctx.outputs.executable,
                command = "{runner} {binary}",
            )
        return [default_info] + extra_providers

    return rule(
        implementation = _impl,
        toolchains = [Label("//private/runner:toolchain_type")],
        attrs = {
            "binary": attr.label(
                executable = True,
                cfg = "target",
                mandatory = True,
            ),
        },
        executable = True,
        test = is_test,
    )

bzd_runner_binary = bzd_runner_factory(is_test = False)
bzd_runner_test = bzd_runner_factory(is_test = True)
