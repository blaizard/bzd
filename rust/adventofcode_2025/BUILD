load("@rules_rust//rust:defs.bzl", "rust_library")
load("//rust:defs.bzl", "rust_binary")

rust_library(
    name = "utils",
    srcs = ["src/utils.rs"],
    deps = ["//rust/libs/std"],
)

[rust_binary(
    name = name,
    srcs = ["src/{}.rs".format(name)],
    args = ["$(location :src/{}.txt)".format(name)],
    data = [
        ":src/{}.txt".format(name),
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
    "day9",
]]
