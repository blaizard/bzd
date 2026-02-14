"""Extension to generate a rust toolchain."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bzd_lib//:repository_multiplatform_maker.bzl", "repository_multiplatform_maker")
load("@bzd_platforms//:defs.bzl", "constraints_from_platform_name")
load("//toolchains:defs.bzl", "config_settings_to_rust_triple")

_repositories = {
    "esp-rust-1.92.0": {
        "stdlib_target": {
            "archive": {
                "build_file": Label("//toolchains/esp_rust_1.92.0:stdlib_target.BUILD"),
                "sha256": "159577235ac78591c850a432ad1a982735fe910d0495d26e7c5c17643acc4b4f",
                "strip_prefix": "rust-src-nightly/rust-src/lib/rustlib/src/rust",
                "urls": ["https://github.com/esp-rs/rust-build/releases/download/v1.92.0.0/rust-src-1.92.0.0.tar.xz"],
            },
        },
        "toolchain": [{
            "archive": {
                "build_file": Label("//toolchains/esp_rust_1.92.0:toolchain.BUILD"),
                "integrity": "sha256-vLCRgGo+El8ParhSFcHrcXT7w4g5dZdlUJ6d0lVl65U=",
                "strip_prefix": "rust-nightly-x86_64-unknown-linux-gnu",
                "urls": [
                    "https://github.com/esp-rs/rust-build/releases/download/v1.92.0.0/rust-1.92.0.0-x86_64-unknown-linux-gnu.tar.xz",
                ],
            },
            "platforms": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
        }],
    },
}

_data = {
    "esp32-xtensa_lx6": {
        "1.92.0": {
            "repository": "esp-rust-1.92.0",
            "target": Label("@bzd_platforms//al_isa:esp32-xtensa_lx6"),
        },
    },
}

def _toolchain_repository_impl(repository_ctx):
    build_content = """
load("@rules_rust//rust:toolchain.bzl", "rust_toolchain", "rust_stdlib_filegroup")
load("{toolchain_defs_bzl}", "use_bootstrap", "toolchain_sysroot", "config_settings_to_rust_triple")

# ---- Toolchain config setting,
""".format(
        toolchain_defs_bzl = Label("//toolchains:defs.bzl"),
    )

    if repository_ctx.attr.toolchain_constraint:
        build_content += """
alias(
    name = "toolchain",
    actual = "{toolchain_constraint}",
    visibility = ["//visibility:public"],
)
""".format(toolchain_constraint = repository_ctx.attr.toolchain_constraint)
    else:
        build_content += """
constraint_value(
    name = "toolchain",
    constraint_setting = "@bzd_platforms//toolchain",
    visibility = ["//visibility:public"],
)
"""

    build_content += """
# ---- Build the target stdlib

use_bootstrap(
    name = "stdlib_target_from_srcs",
    srcs = [
        "@{stdlib_target}//:stdlib_target"
    ]
)

toolchain_sysroot(
    name = "stdlib_target_with_sysroot",
    srcs = [":stdlib_target_from_srcs"],
    dirname = "sysroot_with_stdlibs",
    target_triple = select(config_settings_to_rust_triple),
)

rust_stdlib_filegroup(
    name = "stdlib_target",
    srcs = select({{
        "{toolchain_is_bootstrap}": [],
        "//conditions:default": [":stdlib_target_with_sysroot"],
    }}),
)
""".format(
        toolchain_is_bootstrap = Label("//toolchains:is_bootstrap"),
        stdlib_target = repository_ctx.attr.stdlib_target,
    )

    target_constraints = constraints_from_platform_name(repository_ctx.attr.target_platform.name)
    for index, exec_platform in enumerate(repository_ctx.attr.exec_platforms):
        exec_constraints = constraints_from_platform_name(exec_platform.name)

        build_content += """
# ---- Toolchain for {exec_triple}

rust_toolchain(
    name = "rust_toolchain_{index}",
    binary_ext = "",
    default_edition = "{edition}",
    dylib_ext = ".so",
    env = {{}},
    exec_triple = "{exec_triple}",
    extra_exec_rustc_flags = [],
    extra_rustc_flags = [],
    rust_doc = "@esp32//:rustdoc",
    rust_std = select({{
        "{target_platform}": ":stdlib_target",
        "//conditions:default": "@esp32//:stdlib_exec",
    }}),
    rustc = "@esp32//:rustc",
    staticlib_ext = ".a",
    stdlib_linkflags = [],
    target_triple = select(config_settings_to_rust_triple),
)

toolchain(
    name = "toolchain_target_{index}",
    exec_compatible_with = [
        {exec_constraints}
    ],
    target_compatible_with = [
        ":toolchain",
        {target_constraints}
    ],
    toolchain = "rust_toolchain_{index}",
    toolchain_type = "@rules_rust//rust:toolchain_type",
)

toolchain(
    name = "toolchain_exec_{index}",
    exec_compatible_with = [
        {exec_constraints}
    ],
    target_compatible_with = [
        ":toolchain",
        {exec_constraints}
    ],
    toolchain = ":rust_toolchain_{index}",
    toolchain_type = "@rules_rust//rust:toolchain_type",
)
""".format(
            index = index,
            edition = repository_ctx.attr.edition,
            exec_triple = config_settings_to_rust_triple[exec_platform],
            exec_platform = exec_platform,
            target_platform = repository_ctx.attr.target_platform,
            target_constraints = ", ".join(["\"{}\"".format(constraint) for constraint in target_constraints]),
            exec_constraints = ", ".join(["\"{}\"".format(constraint) for constraint in exec_constraints]),
        )

    repository_ctx.file(
        "BUILD",
        executable = False,
        content = build_content,
    )

toolchain_repository = repository_rule(
    implementation = _toolchain_repository_impl,
    attrs = {
        "edition": attr.string(mandatory = True),
        "exec_platforms": attr.label_list(mandatory = True),
        "repo_name": attr.string(mandatory = True),
        "stdlib_target": attr.string(mandatory = True),
        "target_platform": attr.label(mandatory = True),
        "toolchain": attr.string(mandatory = True),
        "toolchain_constraint": attr.label(),
    },
)

def _toolchain_rust_impl(module_ctx):
    # Gather all defined toolchains and validate them.
    toolchains = {}
    for mod in module_ctx.modules:
        for toolchain in mod.tags.toolchain:
            if toolchain.name in toolchains:
                fail("Duplicate Rust toolchain name '%s'." % toolchain.name)
            if toolchain.version not in _data[toolchain.target]:
                fail("Unsupported Rust version '%s' for target '%s'." % (toolchain.version, toolchain.target))
            toolchains[toolchain.name] = struct(
                edition = toolchain.edition,
                toolchain_constraint = toolchain.toolchain_constraint,
                data = _data[toolchain.target][toolchain.version],
            )
    all_repositories = {toolchain.data["repository"]: True for toolchain in toolchains.values()}.keys()

    # Load the toolchain and stdlib for the target.
    for name in all_repositories:
        repository = _repositories[name]
        repository_multiplatform_maker(
            name = "{}_toolchain".format(name),
            repositories = repository["toolchain"],
            expose = {
                "rustc": "rustc",
                "rustdoc": "rustdoc",
                "stdlib_exec": "stdlib_exec",
            },
        )
        http_archive(
            name = "{}_stdlib_target".format(name),
            **repository["stdlib_target"]["archive"]
        )

    # Make the rust repositories.
    for name, toolchain in toolchains.items():
        repository_name = toolchain.data["repository"]
        repository = _repositories[repository_name]
        exec_platforms = {platform: True for toolchain in repository["toolchain"] for platform in toolchain.get("platforms", [])}.keys()
        toolchain_repository(
            name = name,
            repo_name = name,
            target_platform = toolchain.data["target"],
            exec_platforms = exec_platforms,
            toolchain = "{}_toolchain".format(repository_name),
            stdlib_target = "{}_stdlib_target".format(repository_name),
            edition = toolchain.edition,
            toolchain_constraint = toolchain.toolchain_constraint,
        )

    repository = _repositories["esp-rust-1.92.0"]

    repository_multiplatform_maker(
        name = "esp32",
        repositories = repository["toolchain"],
        expose = {
            "rustc": "rustc",
            "rustdoc": "rustdoc",
            "stdlib_exec": "stdlib_exec",
        },
    )

    http_archive(
        name = "stdlib_target",
        **repository["stdlib_target"]["archive"]
    )

_ALL_VERSIONS = {version: True for value in _data.values() for version in value.keys()}.keys()

toolchain_rust = module_extension(
    implementation = _toolchain_rust_impl,
    tag_classes = {
        "toolchain": tag_class(
            attrs = {
                "edition": attr.string(mandatory = True),
                "name": attr.string(mandatory = True),
                "target": attr.string(values = _data.keys(), mandatory = True),
                "toolchain_constraint": attr.label(doc = "Defines the constraint to select the toolchain."),
                "version": attr.string(values = _ALL_VERSIONS, mandatory = True),
            },
        ),
    },
)
