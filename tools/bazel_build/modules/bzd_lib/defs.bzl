"""Public rules exposed by bzd_lib."""

load("//private/diff_test:diff_test.bzl", _bzd_diff_test = "bzd_diff_test")
load("//private/executable_test:executable_test.bzl", _bzd_executable_test = "bzd_executable_test")
load("//private/runner:runner.bzl", _bzd_runner_binary = "bzd_runner_binary", _bzd_runner_factory = "bzd_runner_factory", _bzd_runner_test = "bzd_runner_test")
load("//private/runner:toolchain.bzl", _bzd_make_runner_toolchain = "bzd_make_runner_toolchain")

bzd_diff_test = _bzd_diff_test

bzd_executable_test = _bzd_executable_test

bzd_runner_binary = _bzd_runner_binary
bzd_runner_test = _bzd_runner_test
bzd_runner_factory = _bzd_runner_factory
bzd_make_runner_toolchain = _bzd_make_runner_toolchain
