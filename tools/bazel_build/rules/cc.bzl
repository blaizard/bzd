load("//tools/bazel_build/rules:bdl.bzl", "bdl_library", "bdl_system", "bdl_system_test")
load("@bazel_skylib//lib:sets.bzl", "sets")
load("//tools/bazel_build/rules/assets/cc:defs.bzl", "cc_compile")
load("@rules_cc//cc:defs.bzl", "cc_library")

def bzd_cc_binary(name, target = "//cc/targets:auto", tags = [], bdls = [], hdrs = [], srcs = [], deps = [], testonly = False, **kwargs):
    """Rule that defines a bzd C++ binary."""

    updated_deps = deps + []
    if bdls:
        bdl_library(
            name = "{}.bdls".format(name),
            tags = tags + ["manual"],
            srcs = bdls,
        )
        updated_deps.append("{}.bdls".format(name))

    if hdrs or srcs:
        cc_library(
            name = "{}.srcs".format(name),
            tags = tags + ["manual", "cc"],
            hdrs = hdrs,
            srcs = srcs,
            deps = updated_deps,
            testonly = testonly,
            alwayslink = True,
        )
        updated_deps.append("{}.srcs".format(name))

    bdl_system(
        name = name,
        targets = {
            "default": target,
        },
        tags = tags + ["cc"],
        deps = updated_deps,
        testonly = testonly,
        **kwargs
    )

def bzd_cc_test(name, target = "//cc/targets:auto", tags = [], bdls = [], hdrs = [], srcs = [], deps = [], testonly = True, **kwargs):
    """Rule that defines a bzd C++ test binary."""

    updated_deps = deps + []
    if bdls:
        bdl_library(
            name = "{}.bdls".format(name),
            tags = tags + ["manual"],
            srcs = bdls,
        )
        updated_deps.append("{}.bdls".format(name))

    if hdrs or srcs:
        cc_library(
            name = "{}.srcs".format(name),
            tags = tags + ["manual", "cc"],
            hdrs = hdrs,
            srcs = srcs,
            deps = updated_deps,
            testonly = testonly,
            alwayslink = True,
        )
        updated_deps.append("{}.srcs".format(name))

    bdl_system_test(
        name = name,
        targets = {
            "default": target,
        },
        tags = tags + ["cc"],
        deps = updated_deps,
        testonly = testonly,
        **kwargs
    )

def _bzd_cc_library_impl(ctx):
    # Build the list of public headers
    hdrs = sets.make(ctx.files.hdrs)
    for dep in ctx.attr.deps:
        for f in dep[CcInfo].compilation_context.direct_public_headers:
            sets.insert(hdrs, f)

    # Build the public header file
    hdrs_file = ctx.actions.declare_file("{}.hh".format(ctx.attr.name))
    ctx.actions.write(
        output = hdrs_file,
        content = "#pragma once\n\n{content}".format(content = "\n".join(["#include \"{}\"".format(f.path) for f in sets.to_list(hdrs)])),
    )

    info_provider = cc_compile(
        ctx = ctx,
        hdrs = ctx.files.hdrs + [hdrs_file],
        srcs = ctx.files.srcs,
        deps = ctx.attr.deps,
    )

    return info_provider

_bzd_cc_library = rule(
    doc = """
    Provide a replacement to cc_library with additional functionalities:
    - Generates a header file containing all hdrs and the ones from the direct deps.
    """,
    implementation = _bzd_cc_library_impl,
    attrs = {
        "deps": attr.label_list(
            doc = "C++ dependencies",
            providers = [CcInfo],
        ),
        "hdrs": attr.label_list(
            doc = "C++ header files",
            allow_files = True,
        ),
        "srcs": attr.label_list(
            doc = "C++ source files",
            allow_files = True,
        ),
        "_cc_toolchain": attr.label(default = Label("@rules_cc//cc:current_cc_toolchain")),
    },
    toolchains = [
        "@rules_cc//cc:toolchain_type",
        "@bzd_toolchain_cc//binary:toolchain_type",
    ],
    fragments = ["cpp"],
)

def bzd_cc_library(tags = [], **kwags):
    """Rule to define a bzd C++ library."""

    _bzd_cc_library(
        tags = tags + ["cc"],
        **kwags
    )
