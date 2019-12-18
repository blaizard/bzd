BzdManifestInfo = provider(fields=["manifest"])

def _bzd_cc_manifest(ctx):
    return [DefaultInfo(), CcInfo(), BzdManifestInfo(manifest = depset(direct = ctx.attr.manifest))]

bzd_cc_manifest = rule(
	implementation = _bzd_cc_manifest,
	attrs = {
		"manifest": attr.label_list(
			allow_files = True,
			mandatory = True,
			doc = "Input manifest files.",
		),
		"deps": attr.label_list(
			allow_files = True,
		)
	},
)

def _bzd_deps_aspect(target, ctx):
    if ctx.rule.kind == "bzd_cc_manifest":
        return []
    return [
		BzdManifestInfo(manifest = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.rule.attr.deps]))
	]

bzd_deps_aspect = aspect(
	implementation = _bzd_deps_aspect,
    attr_aspects = ["deps"]
)

def _bzd_cc_binary(ctx):

    manifest = depset(transitive = [dep[BzdManifestInfo].manifest for dep in ctx.attr.deps]).to_list()
    print(manifest)

    return [DefaultInfo(), CcInfo()]

bzd_cc_binary = rule(
	implementation = _bzd_cc_binary,
	attrs = {
		"deps": attr.label_list(
            aspects = [bzd_deps_aspect],
			doc = "Binary dependencies.",
		),
	},
)
