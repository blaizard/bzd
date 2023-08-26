"""Rules for NodeJs."""

load("@bzd_package//:defs.bzl", "BzdPackageFragmentInfo", "BzdPackageMetadataFragmentInfo", "bzd_package")
load("@bzd_toolchain_nodejs//nodejs:private/nodejs_executable.bzl", bzd_nodejs_binary_ = "bzd_nodejs_binary", bzd_nodejs_test_ = "bzd_nodejs_test")
load("@bzd_toolchain_nodejs//nodejs:private/nodejs_extern_binary.bzl", bzd_nodejs_extern_binary_ = "bzd_nodejs_extern_binary")
load("@bzd_toolchain_nodejs//nodejs:private/nodejs_install.bzl", bzd_nodejs_install_ = "bzd_nodejs_install")
load("@bzd_toolchain_nodejs//nodejs:private/nodejs_library.bzl", bzd_nodejs_library_ = "bzd_nodejs_library")
load("@bzd_toolchain_nodejs//nodejs:private/utils.bzl", "BzdNodeJsDepsInfo", "BzdNodeJsInstallInfo")
load("@bzd_utils//:sh_binary_wrapper.bzl", "sh_binary_wrapper_impl")
load("@rules_oci//oci:defs.bzl", "oci_image", "oci_push")

# Public API.

bzd_nodejs_install = bzd_nodejs_install_
bzd_nodejs_library = bzd_nodejs_library_
bzd_nodejs_extern_binary = bzd_nodejs_extern_binary_

def bzd_nodejs_binary(name, main, args = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs", "manual"],
        **kwargs
    )

    bzd_nodejs_binary_(
        name = name,
        main = main,
        args = args,
        install = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )

def bzd_nodejs_test(name, main, args = [], visibility = [], tags = [], **kwargs):
    bzd_nodejs_install(
        name = name + ".install",
        tags = ["nodejs", "manual"],
        **kwargs
    )

    bzd_nodejs_test_(
        name = name,
        main = main,
        args = args,
        executor = "@bzd_toolchain_nodejs//mocha",
        install = name + ".install",
        tags = ["nodejs"] + tags,
        visibility = visibility,
    )

# ---- _bzd_nodejs_exec

def _bzd_nodejs_exec_impl(ctx, is_test):
    """Implementation of the executor.

    Create the following structure:

    .runfiles/node_modules
        - 3rd party modules
    .runfiles/_main/node_modules
        - apps (local module)
        - nodejs (local)
        - ...

    This works because modules are discovered looking at the first node_modules found, if not found
    it will look in the parents directories.
    """

    # Retrieve install artefacts generated by ctx.attr.dep
    node_modules = ctx.attr.dep[BzdNodeJsInstallInfo].node_modules
    package_json = ctx.attr.dep[BzdNodeJsInstallInfo].package_json

    # Retrieve source files and data
    srcs = ctx.attr.dep[BzdNodeJsDepsInfo].srcs.to_list()
    data = ctx.attr.dep[BzdNodeJsDepsInfo].data.to_list()

    # Gather toolchain executable
    toolchain_executable = ctx.toolchains["@bzd_toolchain_nodejs//nodejs:toolchain_type"].executable

    # Run the node process
    command = "BZD_RULE=nodejs {binary}"

    # Add prefix command to support code coverage
    is_coverage = ctx.configuration.coverage_enabled
    if is_coverage:
        command += " \"{root}/node_modules/c8/bin/c8\" --reporter lcov --reporter text --allowExternal {binary}"

    if is_test:
        command += " \"{root}/node_modules/mocha/bin/mocha\" \"{path}\""
    else:
        command += " \"{path}\" $@"

    if is_coverage:
        command += " && cp \"coverage/lcov.info\" \"$COVERAGE_OUTPUT_FILE\""

    # Generate a 'bzd' module with all source files.
    symlinks = {"node_modules/{}".format(f.short_path): f for f in srcs}

    result = [
        sh_binary_wrapper_impl(
            ctx = ctx,
            locations = {
                toolchain_executable.node: "binary",
                ctx.attr.main: "path",
            },
            output = ctx.outputs.executable,
            command = command,
            extra_runfiles = [node_modules, package_json] + srcs + data,
            root_symlinks = {
                "node_modules": node_modules,
                "package.json": package_json,
            },
            symlinks = symlinks,
        ),
    ]

    if is_coverage:
        result.append(
            coverage_common.instrumented_files_info(
                ctx,
                source_attributes = ["srcs"],
                extensions = ["js", "cjs", "mjs"],
            ),
        )

    if not is_test:
        files_remap = {
            "node_modules": node_modules,
            "package.json": package_json,
        }
        files_remap.update(symlinks)

        result.append(BzdPackageFragmentInfo(
            files = srcs,
            files_remap = files_remap,
        ))

        result.append(BzdPackageMetadataFragmentInfo(
            manifests = [ctx.file._metadata_json],
        ))

    return result

# ---- Docker Package ----

def bzd_nodejs_docker(name, deps, cmd, base = "@docker//:nodejs", include_metadata = False, deploy = {}):
    """Rule for embedding a NodeJs application into Docker.

    Args:
        name: The name of the target.
        deps: The dependencies.
        cmd: The command to be used.
        base: The base image.
        include_metadata: Wether metadata shall be included.
        deploy: The deploy rules.
    """

    bzd_package(
        name = "{}.package".format(name),
        tags = ["nodejs"],
        deps = deps,
        include_metadata = include_metadata,
    )

    root_directory = "/bzd/bin"
    map_to_directory = {dir_name: "{}/{}".format(root_directory, dir_name) for dir_name in deps.values()}

    oci_image(
        name = name,
        base = base,
        cmd = [
            "node",
            "--experimental-json-modules",
        ] + [item.format(**map_to_directory) for item in cmd],
        workdir = root_directory,
        env = {
            "NODE_ENV": "production",
        },
        #stamp = "@io_bazel_rules_docker//stamp:always",
        tags = ["nodejs"],
        tars = [
            "{}.package".format(name),
        ],
    )

    # Add deploy rules if any
    for rule_name, url in deploy.items():
        oci_push(
            name = rule_name,
            image = ":{}".format(name),
            repository = url,
            remote_tags = ["latest"],
            tags = [
                "docker",
            ],
        )
