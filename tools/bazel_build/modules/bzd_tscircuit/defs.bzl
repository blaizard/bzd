"""Rules for tscircuit."""

load(
    "//private:provider.bzl",
    _BzdTscircuitInfo = "BzdTscircuitInfo",
)
load(
    "//private:tscircuit_binary.bzl",
    _bzd_tscircuit_binary = "bzd_tscircuit_binary",
)
load(
    "//private:tscircuit_library.bzl",
    _bzd_tscircuit_library = "bzd_tscircuit_library",
)

BzdTscircuitInfo = _BzdTscircuitInfo

bzd_tscircuit_binary = _bzd_tscircuit_binary
bzd_tscircuit_library = _bzd_tscircuit_library
