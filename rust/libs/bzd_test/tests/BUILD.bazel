load("@bzd_lib//:defs.bzl", "bzd_executable_test")
load("@rules_rust//rust:defs.bzl", "rust_library")
load("//rust:defs.bzl", "rust_test")

# ---- Test success ----

rust_library(
    name = "lib",
    srcs = [
        "extra_file.rs",
        "lib.rs",
    ],
)

rust_test(
    name = "test_crate",
    crate = ":lib",
)

# ---- Test failure ----

rust_library(
    name = "failure",
    srcs = [
        "failure.rs",
    ],
)

rust_test(
    name = "test_failure_executable",
    crate = ":failure",
    tags = ["manual"],
)

bzd_executable_test(
    name = "test_failure",
    executable = ":test_failure_executable",
    unexpected_returncode = "0",
)
