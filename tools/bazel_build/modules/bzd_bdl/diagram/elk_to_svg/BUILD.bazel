load("@bzd_lib//:symlink.bzl", "bzd_symlink")
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_symlink(
    name = "entry_point",
    input = select({
        "@bzd_rules_nodejs//:nodejs": "node.mjs",
        "@bzd_rules_nodejs//:nodejs_web": "window.mjs",
    }),
    output = "entry_point.mjs",
)

bzd_nodejs_library(
    name = "elk_to_svg",
    srcs = [
        "entry_point.mjs",
        "library.mjs",
    ],
    packages = select({
        "@bzd_rules_nodejs//:nodejs": [
            "@bzd_bdl_nodejs_deps//:svgdom",
        ],
        "@bzd_rules_nodejs//:nodejs_web": [],
    }) + [
        "@bzd_bdl_nodejs_deps//:elkjs",
        "@bzd_bdl_nodejs_deps//:svgdotjs-svg-js",
    ],
    visibility = ["//diagram:__pkg__"],
)
