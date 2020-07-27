BzdSettingCacheProvider = provider(fields = ["path"])

def _impl(ctx):
    path = ctx.build_setting_value
    return BzdSettingCacheProvider(path = path)

bzd_setting_cache = rule(
    implementation = _impl,
    build_setting = config.string(flag = True),
)
