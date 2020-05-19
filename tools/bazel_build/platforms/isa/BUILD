package(default_visibility = ["//visibility:public"])

# Instruction Set Architecture, try to match https://en.wikipedia.org/wiki/Comparison_of_instruction_set_architectures
constraint_setting(name = "isa")

constraint_value(
    name = "x86_32",
    constraint_setting = ":isa",
)

constraint_value(
    name = "x86_64",
    constraint_setting = ":isa",
)

constraint_value(
    name = "xtensa_lx6",
    constraint_setting = ":isa",
)
