"""Public rules exposed by bzd_lib."""

load("//private/diff_test:diff_test.bzl", _bzd_diff_test = "bzd_diff_test")
load("//private/executable_test:executable_test.bzl", _bzd_executable_exec_test = "bzd_executable_exec_test", _bzd_executable_test = "bzd_executable_test")

bzd_diff_test = _bzd_diff_test
bzd_executable_test = _bzd_executable_test
bzd_executable_exec_test = _bzd_executable_exec_test
