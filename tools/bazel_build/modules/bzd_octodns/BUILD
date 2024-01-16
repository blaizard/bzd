load("@bzd_octodns_pip//:requirements.bzl", "requirement")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

py_console_script_binary(
    name = "octodns-sync",
    pkg = "@bzd_octodns_pip//octodns",
    deps = [
        requirement("octodns_digitalocean"),
    ],
)

py_console_script_binary(
    name = "octodns-validate",
    pkg = "@bzd_octodns_pip//octodns",
    deps = [
        requirement("octodns_digitalocean"),
    ],
)
