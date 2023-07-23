package(default_visibility = ["//visibility:public"])

load("@bzd_platforms//:defs.bzl", "AL_CONSTRAINTS")

alias(
    name = "al",
    actual = "@platforms//os",
)

[alias(
    name = al,
    actual = actual,
) for al, actual in AL_CONSTRAINTS.items() if actual]

[constraint_value(
    name = al,
    constraint_setting = ":al",
) for al, actual in AL_CONSTRAINTS.items() if not actual]
