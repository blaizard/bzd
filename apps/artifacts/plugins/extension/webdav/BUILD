load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

bzd_nodejs_library(
    name = "webdav",
    srcs = [
        "webdav.mjs",
    ],
    packages = [
        "@nodejs_deps//:fast-xml-parser",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//nodejs/db/storage",
        "//nodejs/utils:pathlib",
    ],
)

py_console_script_binary(
    name = "dav",
    pkg = "@pip_linux_x86_64//webdav4",
    script = "dav",
    tags = ["manual"],
    visibility = ["//visibility:public"],
    deps = [
        "@pip_linux_x86_64//fsspec",
    ],
)
