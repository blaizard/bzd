load("@bzd_dns_pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

py_console_script_binary(
    name = "octodns-sync",
    pkg = "@bzd_dns_pip//octodns",
    visibility = ["//visibility:public"],
    deps = [
        requirement("octodns_digitalocean"),
        requirement("octodns_bind"),
    ],
)

py_console_script_binary(
    name = "octodns-validate",
    pkg = "@bzd_dns_pip//octodns",
    visibility = ["//visibility:public"],
    deps = [
        requirement("octodns_digitalocean"),
        requirement("octodns_bind"),
    ],
)

py_library(
    name = "lib",
    srcs = [
        "octodns.py",
        "record.py",
        "zones.py",
    ],
    imports = [".."],
    deps = [
        requirement("pyyaml"),
    ],
)

py_binary(
    name = "aggregator",
    srcs = [
        "aggregator.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ],
)

py_binary(
    name = "build",
    srcs = [
        "build.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ],
)
