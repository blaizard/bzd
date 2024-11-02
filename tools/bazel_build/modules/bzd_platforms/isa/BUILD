load("//:constraints.bzl", "ISA_CONSTRAINTS")

package(default_visibility = ["//visibility:public"])

# Instruction Set Architecture, try to match https://en.wikipedia.org/wiki/Comparison_of_instruction_set_architectures
alias(
    name = "isa",
    actual = "@platforms//cpu",
)

[alias(
    name = isa,
    actual = data["alias"],
) for isa, data in ISA_CONSTRAINTS.items() if data.get("alias") != None]

[constraint_value(
    name = isa,
    constraint_setting = ":isa",
) for isa, data in ISA_CONSTRAINTS.items() if data.get("alias") == None]
