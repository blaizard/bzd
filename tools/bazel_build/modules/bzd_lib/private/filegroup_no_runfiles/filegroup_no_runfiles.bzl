"""A filegroup implementation excluding the runfiles."""

def _bzd_filegroup_no_runfiles(ctx):
    if ctx.attr.output_group:
        files = depset(transitive = [src[OutputGroupInfo][ctx.attr.output_group] for src in ctx.attr.srcs])
    else:
        files = depset(transitive = [src[DefaultInfo].files for src in ctx.attr.srcs])
    return [DefaultInfo(files = files)]

bzd_filegroup_no_runfiles = rule(
    implementation = _bzd_filegroup_no_runfiles,
    attrs = {
        "output_group": attr.string(),
        "srcs": attr.label_list(
            allow_files = True,
        ),
    },
)
