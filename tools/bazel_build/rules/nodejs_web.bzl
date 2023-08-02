"""Rules for nodejs_web."""

load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("//tools/bazel_build/rules:nodejs.bzl", "BzdNodeJsDepsProvider", "BzdNodeJsInstallProvider", "bzd_nodejs_aliases_symlinks", "bzd_nodejs_install")
load("//tools/bazel_build/rules:package.bzl", "BzdPackageFragmentInfo", "BzdPackageMetadataFragmentInfo")

BzdNodeJsWebInfo = provider("Provider for NodeJs web information.", fields = ["tar"])

def _bzd_nodejs_web_build_impl(ctx):
    # Retrieve install artefacts generated by ctx.attr.dep
    node_modules = ctx.attr.dep[BzdNodeJsInstallProvider].node_modules
    package_json = ctx.attr.dep[BzdNodeJsInstallProvider].package_json
    aliases = ctx.attr.dep[BzdNodeJsInstallProvider].aliases

    # Retrieve source files
    srcs = ctx.attr.dep[BzdNodeJsDepsProvider].srcs.to_list()

    # --- Generate the webpack.config.json file
    webpack_config = ctx.actions.declare_file("webpack.config.js")

    # entries and templates
    entries = ""
    templates = ""
    for entry_point, name in ctx.attr.entries.items():
        entry_point_files = entry_point.files.to_list()
        if len(entry_point_files) != 1:
            fail("Only a single file should be associated with entry '{}'.".format(name))
        entries += "\"{}\": Path.join(Path.resolve(__dirname), \"{}\"),\n".format(name, entry_point_files[0].short_path)

        # For HTML templates
        templates += "\"{}\": {{ entryId: \"{}\" }},".format(name, name)

    # Identify the build mode
    mode = None
    if ctx.attr.build_type == "prod":
        mode = "production"
    elif ctx.attr.build_type == "dev":
        mode = "development"
    else:
        fail("Invalid build type, only values 'prod' or 'dev' are supported.")

    # Generate the template file
    ctx.actions.expand_template(
        template = ctx.file._webpack_config_template,
        output = webpack_config,
        substitutions = {
            "{entries}": entries,
            "{mode}": mode,
            "{public_path}": ctx.attr.public_path,
            "{templates}": templates,
        },
    )

    # Gather toolchain manager
    toolchain_executable = ctx.toolchains["//tools/bazel_build/toolchains/nodejs:toolchain_type"].executable

    # Create the symlinks for the aliases
    symlinks = bzd_nodejs_aliases_symlinks(files = srcs, aliases = aliases)
    symlinks.update({
        "webpack.config.cjs": webpack_config,
    })

    # Create a wrapped binary to have a self contained execution environment
    return [
        sh_binary_wrapper_impl(
            ctx = ctx,
            binary = toolchain_executable.node,
            output = ctx.outputs.executable,
            command = """
            export BZD_RULE=nodejs_web
            {binary} "{root}/node_modules/.bin/webpack" --config "{workspace}/webpack.config.cjs" --output-path ".bzd/output"
            tar -h -cf "$1" -C "{workspace}/.bzd/output" --transform 's,^./,,' .
            """,
            extra_runfiles = [webpack_config, package_json, node_modules] + srcs,
            root_symlinks = {
                "node_modules": node_modules,
                "package.json": package_json,
            },
            symlinks = symlinks,
        ),
    ]

_bzd_nodejs_web_build = rule(
    doc = "Build and package a web application with webpack.",
    implementation = _bzd_nodejs_web_build_impl,
    attrs = {
        "alias": attr.string(
            doc = "Name of the alias, available in the form [name], for the current directory.",
        ),
        "build_type": attr.string(
            mandatory = True,
            doc = "Build type eth for production or development.",
        ),
        "dep": attr.label(
            mandatory = True,
        ),
        "entries": attr.label_keyed_string_dict(
            allow_files = True,
            doc = "Associate entry names with source entry point.",
        ),
        "public_path": attr.string(
            default = "/",
            doc = "Defines from which path the user will access these entries.",
        ),
        "target_name": attr.string(
            mandatory = True,
            doc = "Name of actual target.",
        ),
        "_webpack_config_template": attr.label(
            default = Label("//tools/bazel_build/rules/assets/nodejs:webpack_config_template"),
            allow_single_file = True,
        ),
    },
    executable = True,
    toolchains = ["//tools/bazel_build/toolchains/nodejs:toolchain_type"],
)

def _bzd_nodejs_web_package_impl(ctx):
    # Build the application
    package = ctx.actions.declare_file(".bzd/nodejs_web_{}.tar".format(ctx.label.name))
    ctx.actions.run(
        inputs = [],
        outputs = [package],
        arguments = [package.path],
        progress_message = "Building {}".format(ctx.label),
        mnemonic = "NodejsPackage",
        executable = ctx.executable.dep,
    )
    return [
        DefaultInfo(files = depset([package])),
        BzdNodeJsWebInfo(tar = package),
    ]

_bzd_nodejs_web_package = rule(
    doc = "NodeJs web application packaging.",
    implementation = _bzd_nodejs_web_package_impl,
    attrs = {
        "dep": attr.label(
            executable = True,
            cfg = "exec",
        ),
        "target_name": attr.string(
            mandatory = True,
            doc = "Name of actual target.",
        ),
    },
)

def _bzd_nodejs_web_exec_impl(ctx):
    package = ctx.attr.dep[BzdNodeJsWebInfo].tar

    # Run the application
    return [
        sh_binary_wrapper_impl(
            ctx = ctx,
            binary = ctx.attr._web_server,
            output = ctx.outputs.executable,
            command = "{{binary}} $@ \"{}\"".format(package.short_path),
            extra_runfiles = [package],
        ),
        BzdPackageFragmentInfo(
            tars = [
                package,
            ],
        ),
        BzdPackageMetadataFragmentInfo(
            manifests = [ctx.file._metadata_json],
        ),
    ]

_bzd_nodejs_web_exec = rule(
    doc = "NodeJs web application executor.",
    implementation = _bzd_nodejs_web_exec_impl,
    attrs = {
        "dep": attr.label(
        ),
        "_metadata_json": attr.label(
            default = Label("//tools/bazel_build/rules/assets/nodejs_web:metadata_json"),
            allow_single_file = True,
        ),
        "_web_server": attr.label(
            executable = True,
            cfg = "target",
            allow_files = True,
            default = Label("//tools/scripts:web_server"),
        ),
    },
    executable = True,
)

def bzd_nodejs_web(name, aliases = {}, srcs = [], packages = {}, deps = [], visibility = [], **kwargs):
    """Public macro to create a web application with NodeJs.

    Args:
        name: The name of the target.
        aliases: The aliases.
        srcs: The source files.
        packages: The packages to add.
        deps: The dependencies.
        visibility: The visibility for the target.
        **kwargs: Additional arguments to add to the rule.
    """

    # Gather dependencies and install the packages
    bzd_nodejs_install(
        name = name + ".install",
        aliases = aliases,
        srcs = srcs,
        packages = packages,
        deps = deps + [
            "//nodejs:webpack",
        ],
        tags = ["nodejs"],
    )

    # Build the web application and packs it
    _bzd_nodejs_web_build(
        name = name + ".build",
        target_name = name,
        dep = name + ".install",
        tags = ["nodejs"],
        build_type = select({
            "//tools/bazel_build/settings/build:dev": "dev",
            "//tools/bazel_build/settings/build:prod": "prod",
        }),
        **kwargs
    )

    # Generate the package
    _bzd_nodejs_web_package(
        name = name + ".package",
        dep = name + ".build",
        target_name = name,
        tags = ["nodejs"],
        visibility = visibility,
    )

    # Generate the executable
    _bzd_nodejs_web_exec(
        name = name,
        dep = name + ".package",
        tags = ["nodejs"],
        visibility = visibility,
    )
