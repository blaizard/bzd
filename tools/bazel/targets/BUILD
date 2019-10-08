package(default_visibility = ["//visibility:public"])

# Abstraction Layer
constraint_setting(name = "al")
constraint_value(name = "linux", constraint_setting = ":al")

# Instruction Set Architecture, try to match https://en.wikipedia.org/wiki/Comparison_of_instruction_set_architectures
constraint_setting(name = "isa")
constraint_value(name = "x86_32", constraint_setting = ":isa")
constraint_value(name = "x86_64", constraint_setting = ":isa")

# Compilers
constraint_setting(name = "compiler")
constraint_value(name = "gcc", constraint_setting = ":compiler")
constraint_value(name = "clang", constraint_setting = ":compiler")

platform(
    name = "linux_x86_64",
    constraint_values = [
		"@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":linux",
        ":x86_64",
    ],
)
