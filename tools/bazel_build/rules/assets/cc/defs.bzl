load("@rules_cc//cc:find_cc_toolchain.bzl", original_find_cc_toolchain = "find_cc_toolchain")

_CC_SRCS_EXTENSIONS = (".c", ".cc", ".cpp", ".cxx", ".c++", ".C")
_CC_HDRS_EXTENSIONS = (".h", ".hh", ".hpp", ".hxx", ".inc", ".inl", ".H")

def find_cc_toolchain(ctx):
    # TODO use find_cc_toolchain, somehow it doesn't work for all configurations.
    # See this related issue: https://github.com/bazelbuild/rules_cc/issues/74
    cc_toolchain = ctx.toolchains["@rules_cc//cc:toolchain_type"]
    if not hasattr(cc_toolchain, "all_files"):
        cc_toolchain = original_find_cc_toolchain(ctx)
    return cc_toolchain

def cc_compile(ctx, hdrs = [], srcs = [], deps = []):
    """
    Compile header, source and CcInfo dependencies and return a CcInfo.
    """

    cc_toolchain = find_cc_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    cc_infos = [dep[CcInfo] for dep in deps if CcInfo in dep]
    compilation_context, cc_outputs = cc_common.compile(
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        srcs = [f for f in srcs if f.basename.endswith(_CC_SRCS_EXTENSIONS)],
        public_hdrs = hdrs,
        private_hdrs = [f for f in srcs if f.basename.endswith(_CC_HDRS_EXTENSIONS)],
        compilation_contexts = [cc_info.compilation_context for cc_info in cc_infos],
        name = ctx.attr.name,
    )

    linking_context, _ = cc_common.create_linking_context_from_compilation_outputs(
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        compilation_outputs = cc_outputs,
        linking_contexts = [cc_info.linking_context for cc_info in cc_infos],
        name = ctx.attr.name,
    )

    return CcInfo(compilation_context = compilation_context, linking_context = linking_context)

def cc_link(ctx, cc_info):
    # Work in progress

    cc_toolchain = find_cc_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )

    map_file = ctx.actions.declare_file("{}.map".format(ctx.attr.name))
    linking_outputs = cc_common.link(
        name = ctx.attr.name,
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        linking_contexts = [cc_info.linking_context],
        additional_outputs = [map_file],
        user_link_flags = ["-Wl,-Map={}".format(map_file.path)],
    )

    return linking_outputs, map_file
