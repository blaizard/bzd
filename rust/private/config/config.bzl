"""Generate a Rust library from a bzd configuration."""

load("@bzd_lib//config:defs.bzl", "ConfigInfo")
load("@rules_python//python:defs.bzl", "py_binary")
load("@rules_rust//rust:defs.bzl", "rust_library")

def _bzd_rust_config_generator_impl(ctx):
    config = ctx.attr.config[ConfigInfo]
    output = ctx.actions.declare_file(ctx.label.name + ".rs")
    args = ctx.actions.args()
    args.add("--output", output)
    args.add("--internal", config.internal)
    args.add("--expose", json.encode(ctx.attr.expose))
    args.add("--types", json.encode(ctx.attr.types))
    ctx.actions.run(
        inputs = [config.internal],
        outputs = [output],
        arguments = [args],
        executable = ctx.executable.generator,
    )
    return [DefaultInfo(files = depset([output]))]

_bzd_rust_config_generator = rule(
    implementation = _bzd_rust_config_generator_impl,
    attrs = {
        "config": attr.label(providers = [ConfigInfo], mandatory = True),
        "expose": attr.string_dict(mandatory = True),
        "generator": attr.label(
            mandatory = True,
            cfg = "exec",
            executable = True,
        ),
        "types": attr.string_dict(mandatory = True),
    },
)

def _bzd_rust_config_impl(name, visibility, config, expose, types, crate_name, **kwargs):
    py_binary(
        name = "{}.generator".format(name),
        srcs = [Label("//rust/private/config:config_generator.py")],
        main = Label("//rust/private/config:config_generator.py"),
        data = [config],
        deps = [
            Label("@bzd_lib//config:reader"),
            Label("@bzd_python//bzd/utils:runfiles"),
        ],
        tags = ["manual"],
    )
    _bzd_rust_config_generator(
        name = "{}.generate".format(name),
        config = config,
        expose = expose,
        types = types,
        generator = ":{}.generator".format(name),
        tags = ["manual"],
    )
    rust_library(
        name = name,
        srcs = [":{}.generate".format(name)],
        crate_name = crate_name or name.replace("-", "_").replace(".", "_"),
        visibility = visibility,
        **kwargs
    )

bzd_rust_config = macro(
    implementation = _bzd_rust_config_impl,
    inherit_attrs = "common",
    doc = "Generate a Rust library exposing configuration values. `types` must contain every " +
          "exposed key and supports str, bool, usize, i64, f64 and bytes.",
    attrs = {
        "config": attr.label(
            providers = [ConfigInfo],
            mandatory = True,
            configurable = False,
        ),
        "crate_name": attr.string(default = ""),
        "expose": attr.string_dict(mandatory = True),
        "types": attr.string_dict(mandatory = True),
    },
)
