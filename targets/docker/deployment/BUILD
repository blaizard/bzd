load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "base",
    srcs = [
        "base.py",
    ],
    deps = [
        "@bzd_bdl//bdl/generators/json/ast",
    ],
)

py_library(
    name = "compose",
    srcs = [
        "compose.py",
    ],
    visibility = ["//targets/docker:__pkg__"],
    deps = [
        ":base",
        requirement("pyyaml"),
    ],
)

py_library(
    name = "traefik",
    srcs = [
        "traefik.py",
    ],
    data = [
        "traefik.yml.btl",
    ],
    visibility = ["//targets/docker:__pkg__"],
    deps = [
        ":base",
        "@bzd_python//bzd/template",
    ],
)
