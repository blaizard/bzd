load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

_PLUGINS = [
    "fs",
    "docker",
    "webdav",
    "bzd",
]

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "frontend.mjs",
    ],
    visibility = [
        "//apps/artifacts:__subpackages__",
    ],
    deps = ["//apps/artifacts/plugins/{}:frontend".format(name) for name in _PLUGINS],
)

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
    ],
    visibility = [
        "//apps/artifacts:__subpackages__",
    ],
    deps = ["//apps/artifacts/plugins/{}:backend".format(name) for name in _PLUGINS],
)

bzd_nodejs_library(
    name = "default",
    srcs = [
        "base.vue",
        "default.vue",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/utils:to_string",
        "//nodejs/vue/directives:tooltip",
    ],
)
