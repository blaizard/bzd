"""Runner rules."""

load("//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//private/runner:providers.bzl", "BzdRunnerOptionsInfo")

def _bzd_runner_options_impl(ctx):
    files = {}
    for name, target in ctx.attr.files.items():
        targets = target[DefaultInfo].files.to_list()
        if len(targets) != 1:
            fail("Runner option '{}' must contain exactly 1 file, not {}.".format(name, len(targets)))
        files[name] = targets[0]
    return [BzdRunnerOptionsInfo(files = files, values = ctx.attr.values)]

bzd_runner_options = rule(
    implementation = _bzd_runner_options_impl,
    doc = "Define the options passed to a runner through environment variables.",
    attrs = {
        "files": attr.string_keyed_label_dict(
            allow_files = True,
            doc = "Environment variable names to the files they reference.",
        ),
        "values": attr.string_dict(
            doc = "Environment variable names to their values.",
        ),
    },
)

def _get_options(target):
    """Return the runner options of a target, or None if it has none."""

    if target == None or BzdRunnerOptionsInfo not in target:
        return None
    info = target[BzdRunnerOptionsInfo]
    if not info.files and not info.values:
        return None
    return info

def _environment(info):
    """Return the locations and the environment assignments of the runner options."""

    locations = {}
    environment = {}
    for name, value in info.values.items():
        environment[name] = '{}="{}"'.format(name, value)
    for name, file in info.files.items():
        locations[file] = name
        environment[name] = '{}="{{{}}}"'.format(name, name)
    return locations, environment

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
            info = _get_options(ctx.attr.runner_options)

            provided = {}
            if info != None:
                provided = {name: None for name in info.files} | {name: None for name in info.values}
            missing = [name for name in toolchain.required_options if name not in provided]
            unexpected = [name for name in provided if name not in toolchain.required_options]
            if missing or unexpected:
                fail("Runner {}: missing options [{}], unexpected options [{}] (target {}).".format(
                    toolchain.runner.label,
                    ", ".join(missing),
                    ", ".join(unexpected),
                    ctx.label,
                ))

            locations = {
                toolchain.runner: "runner",
                ctx.attr.binary: "binary",
            }
            command = "{runner} {binary}"
            if info != None:
                option_locations, environment = _environment(info)
                locations.update(option_locations)
                command = "".join([assignment + " " for assignment in environment.values()]) + command
            default_info = sh_binary_wrapper_impl(
                ctx = ctx,
                locations = locations,
                output = ctx.outputs.executable,
                command = command,
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
            "runner_options": attr.label(
                providers = [BzdRunnerOptionsInfo],
                doc = "Options passed to the runner.",
            ),
        },
        executable = True,
        test = is_test,
    )

bzd_runner_binary = bzd_runner_factory(is_test = False)
bzd_runner_test = bzd_runner_factory(is_test = True)
