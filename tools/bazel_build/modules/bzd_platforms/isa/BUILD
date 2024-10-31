load("//:defs.bzl", "ISA_CONSTRAINTS")

package(default_visibility = ["//visibility:public"])

# Instruction Set Architecture, try to match https://en.wikipedia.org/wiki/Comparison_of_instruction_set_architectures
alias(
    name = "isa",
    actual = "@platforms//cpu",
)

[alias(
    name = isa,
    actual = actual,
) for isa, actual in ISA_CONSTRAINTS.items() if actual]

[constraint_value(
    name = isa,
    constraint_setting = ":isa",
) for isa, actual in ISA_CONSTRAINTS.items() if not actual]
