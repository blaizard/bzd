package(default_visibility = ["//visibility:public"])

load("@bzd_platforms//:defs.bzl", "AL_CONSTRAINTS", "ISA_CONSTRAINTS")

[config_setting(
    name = "{}-{}".format(al, isa),
    constraint_values = [
        "@bzd_platforms//al:{}".format(al),
        "@bzd_platforms//isa:{}".format(isa),
    ],
) for al in AL_CONSTRAINTS.keys() for isa in ISA_CONSTRAINTS.keys()]
