load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library")

rust_library(
    name = "utils",
    srcs = ["utils.rs"],
)

[rust_binary(
    name = name,
    srcs = ["{}.rs".format(name)],
    args = ["$(location :{}.txt)".format(name)],
    data = [
        ":{}.txt".format(name),
    ],
    deps = [
        ":utils",
    ],
) for name in [
    "day1",
    "day2",
    "day3",
    "day4",
    "day5",
    "day6",
    "day7",
    "day8",
]]
