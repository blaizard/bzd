load("//:defs.bzl", "AL_CONSTRAINTS", "ISA_CONSTRAINTS")

package(default_visibility = ["//visibility:public"])

[config_setting(
    name = "{}-{}".format(al, isa),
    constraint_values = [
        "//al:{}".format(al),
        "//isa:{}".format(isa),
    ],
) for al in AL_CONSTRAINTS.keys() for isa in ISA_CONSTRAINTS.keys()]
