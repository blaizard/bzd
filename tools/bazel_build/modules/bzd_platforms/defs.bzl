AL_CONSTRAINTS = {
    "linux": "@platforms//os:linux",
    "esp32": None,
}

ISA_CONSTRAINTS = {
    "x86_64": "@platforms//cpu:x86_64",
    "xtensa_lx6": None,
}

def _constraint_target(kind, name):
    """Create the constraint target string."""

    return "@bzd_platforms//{}:{}".format(kind, name)

def constraints_from_platform(platform):
    """Return the set of constraints from a platform name."""

    constraints = []
    al, isa = platform.split("-")

    if al in AL_CONSTRAINTS:
        constraints.append(_constraint_target("al", al))
    else:
        fail("Abstraction layer '{}' not supported.".format(al))

    if isa in ISA_CONSTRAINTS:
        constraints.append(_constraint_target("isa", isa))
    else:
        fail("Instruction set architecture '{}' not supported.".format(isa))

    return constraints
