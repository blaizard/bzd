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
        "/tmp/bzd/frontend",
    ],
    main = "app.mjs",
    packages = {
        "commander": "",
        "ws": "",
    },
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
        "//nodejs/core/api:server",
        "//nodejs/core/http:server",
        "//nodejs/db/storage:disk",
    ],
)
