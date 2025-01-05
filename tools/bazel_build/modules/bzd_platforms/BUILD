load("//:constraints.bzl", "AL_CONSTRAINTS")

package(default_visibility = ["//visibility:public"])

[platform(
    name = "{}-{}".format(al, isa),
    constraint_values = [
        "//al:{}".format(al),
        "//isa:{}".format(isa),
    ],
) for al, constraints in AL_CONSTRAINTS.items() for isa in constraints["isa"]]
