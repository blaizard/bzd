"""Macros to generate static files."""

load("@bzd_rules_nodejs//nodejs:private/nodejs_static.bzl", "bzd_nodejs_static")

def bzd_nodejs_static_sitemap(name, config, config_attr, install, format):
    bzd_nodejs_static(
        name = name,
        args = [
            "--output",
            "{output}",
            "--format",
            format,
            "--hostname",
            "{config:" + config_attr + "}",
            "{api}",
        ],
        config = config,
        install = install,
        script = "@bzd_rules_nodejs//nodejs/static:sitemap",
    )
