"""Helper for the generated rust toolchain."""

def _use_bootstrap_transition_impl(_, __):
    return {
        "//toolchains:bootstrap": True,
    }

_use_bootstrap_transition = transition(
    inputs = [],
    outputs = [
        "//toolchains:bootstrap",
    ],
    implementation = _use_bootstrap_transition_impl,
)

def _use_bootstrap_impl(ctx):
    return [DefaultInfo(files = depset(ctx.files.srcs))]

use_bootstrap = rule(
    implementation = _use_bootstrap_impl,
    attrs = {
        "srcs": attr.label_list(
            allow_files = True,
        ),
        "_allowlist_function_transition": attr.label(
            default = Label("@bazel_tools//tools/allowlists/function_transition_allowlist"),
        ),
    },
    cfg = _use_bootstrap_transition,
)

def _toolchain_sysroot_impl(ctx):
    sysroot = ctx.attr.dirname
    outputs = []

    rustlibdir = "{}/lib/rustlib/{}/lib".format(sysroot, ctx.attr.target_triple)

    for inp in ctx.files.srcs:
        out = ctx.actions.declare_file(rustlibdir + "/" + inp.basename)
        outputs.append(out)
        ctx.actions.symlink(output = out, target_file = inp)

    return [DefaultInfo(
        files = depset(outputs),
        runfiles = ctx.runfiles(files = outputs),
    )]

toolchain_sysroot = rule(
    implementation = _toolchain_sysroot_impl,
    attrs = {
        "dirname": attr.string(
            mandatory = True,
        ),
        "srcs": attr.label_list(
            allow_files = True,
        ),
        "target_triple": attr.string(
            doc = "The target triple for the rlibs.",
            default = "x86_64-unknown-linux-gnu",
        ),
    },
)

config_settings_to_rust_triple = {
    Label("@bzd_platforms//al_isa:linux-x86_64"): "x86_64-unknown-linux-gnu",
    Label("@bzd_platforms//al_isa:esp32_idf-xtensa_lx6"): "xtensa-esp32-none-elf",
}
