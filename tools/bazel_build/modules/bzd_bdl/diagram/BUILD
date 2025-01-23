load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
    name = "diagram",
    srcs = [
        "bdl_to_elk.mjs",
        "diagram.mjs",
        "elk_to_svg.mjs",
    ],
    main = "diagram.mjs",
    packages = [
        "@bzd_bdl_nodejs_deps//:commander",
        "@bzd_bdl_nodejs_deps//:elkjs",
        "@bzd_bdl_nodejs_deps//:svgdom",
        "@bzd_bdl_nodejs_deps//:svgdotjs-svg-js",
    ],
    visibility = ["//actions:__subpackages__"],
)
