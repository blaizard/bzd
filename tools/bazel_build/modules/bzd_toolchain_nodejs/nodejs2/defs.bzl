"""Rules for NodeJs."""

def _bzd_nodejs_binary_impl():
    pass

bzd_nodejs_binary = rule(
    doc = "NodeJs web application executor.",
    implementation = _bzd_nodejs_binary_impl,
    attrs = {
        "data": attr.label_list(
            allow_files = True,
            doc = "Data to be added to the runfile list",
        ),
        "deps": attr.label_list(
            aspects = [bzd_nodejs_deps_aspect],
            allow_files = True,
            doc = "Dependencies",
        ),
        "packages": attr.string_dict(
            allow_empty = True,
            doc = "Package dependencies",
        ),
        "srcs": attr.label_list(
            allow_files = True,
            doc = "Source files",
        ),
    },
    executable = True,
)
