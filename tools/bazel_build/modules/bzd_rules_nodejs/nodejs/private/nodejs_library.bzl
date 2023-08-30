"""NodeJs library rule."""

load("@bzd_rules_nodejs//nodejs:private/nodejs_install.bzl", "COMMON_ATTRS")
load("@bzd_rules_nodejs//nodejs:private/utils.bzl", "BzdNodeJsDepsInfo", "bzd_nodejs_make_provider", "bzd_nodejs_merge")

def _bzd_nodejs_library_impl(ctx):
    rule_provider = bzd_nodejs_make_provider(ctx)
    provider = bzd_nodejs_merge(rule_provider, *[d[BzdNodeJsDepsInfo] for d in ctx.attr.deps])

    return [DefaultInfo(), provider]

bzd_nodejs_library = rule(
    doc = "A library contains all depdencies used for this target.",
    implementation = _bzd_nodejs_library_impl,
    attrs = COMMON_ATTRS,
)
