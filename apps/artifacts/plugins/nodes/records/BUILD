load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "records",
    srcs = [
        "distributed.mjs",
        "reader.mjs",
        "records.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:lock",
        "//nodejs/core:log",
        "//nodejs/core/statistics:provider",
    ],
)
