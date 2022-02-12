load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain")

_CC_SRCS_EXTENSIONS = (".c", ".cc", ".cpp", ".cxx", ".c++", ".C")
_CC_HDRS_EXTENSIONS = (".h", ".hh", ".hpp", ".hxx", ".inc", ".inl", ".H")

def _cc_config(ctx):
    """
    Helper function to gather toolchain and feature config
    """

    cc_toolchain = find_cpp_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )
    return cc_toolchain, feature_configuration

def _cc_compile(ctx, hdrs = [], srcs = [], deps = []):
    """
    Compile header, source and dependencies and return internal artifacts.
    """

    cc_toolchain, feature_configuration = _cc_config(ctx)

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

    return compilation_context, cc_outputs, cc_infos

def cc_compile(ctx, hdrs = [], srcs = [], deps = []):
    """
    Compile header, source and dependencies and return a CcInfo.
    """

    cc_toolchain, feature_configuration = _cc_config(ctx)
    compilation_context, cc_outputs, cc_infos = _cc_compile(ctx, hdrs, srcs, deps)

    linking_context, _ = cc_common.create_linking_context_from_compilation_outputs(
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        compilation_outputs = cc_outputs,
        linking_contexts = [cc_info.linking_context for cc_info in cc_infos],
        name = ctx.attr.name,
    )

    return CcInfo(compilation_context = compilation_context, linking_context = linking_context)

def cc_link(ctx, hdrs = [], srcs = [], deps = [], map_analyzer = None):
    """
    Compile and link.
    """

    cc_toolchain, feature_configuration = _cc_config(ctx)
    compilation_context, cc_outputs, cc_infos = _cc_compile(ctx, hdrs, srcs, deps)

    map_file = ctx.actions.declare_file("{}.map".format(ctx.attr.name))
    linking_outputs = cc_common.link(
        name = ctx.attr.name + ".binary",
        actions = ctx.actions,
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        compilation_outputs = cc_outputs,
        linking_contexts = [cc_info.linking_context for cc_info in cc_infos],
        additional_outputs = [map_file],
        user_link_flags = ["-Wl,-Map={}".format(map_file.path)],
    )
    binary_file = linking_outputs.executable
    metadata_files = []

    # Analyze the map file.
    if map_analyzer:
        metadata_file = ctx.actions.declare_file("{}.map.metadata".format(ctx.attr.name))
        ctx.actions.run(
            inputs = [map_file, binary_file],
            outputs = [metadata_file],
            progress_message = "Generating metadata for {}".format(ctx.attr.name),
            arguments = ["--output", metadata_file.path, "--binary", binary_file.path, map_file.path],
            tools = map_analyzer.data_runfiles.files,
            executable = map_analyzer.files_to_run,
        )
        metadata_files.append(metadata_file)

    return binary_file, metadata_files
