load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "terminal",
    srcs = [
        "terminal.py",
    ],
)

bzd_nodejs_binary(
    name = "backend",
    srcs = glob([
        "*.mjs",
    ]),
    args = [
        "--static",
        "$(rootpath //apps/player/frontend).bundle",
    ],
    data = [
        "//apps/player/frontend",
    ],
    main = "app.mjs",
    packages = [
        "@nodejs_deps//:commander",
        "@nodejs_deps//:ws",
    ],
    tools = [
        ":terminal",
    ],
    visibility = [
        "//apps/player:__pkg__",
    ],
    deps = [
        "//apps/player:api",
        "//apps/player/lib:scenario",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core/http:server",
        "//nodejs/core/rest:server",
        "//nodejs/db/storage:disk",
    ],
)
