"""Platform helpers."""

AL_CONSTRAINTS = {
    "esp32": None,
    "linux": "@platforms//os:linux",
}

ISA_CONSTRAINTS = {
    "arm64": "@platforms//cpu:arm64",
    "x86_64": "@platforms//cpu:x86_64",
    "xtensa_lx6": None,
}

def _constraint_target(kind, name):
    """Create the constraint target string.

    Args:
        kind: The kind of constraint.
        name: The name of the constraint.

    Returns:
        A string corresponding to the constraint target.
    """

    return "@bzd_platforms//{}:{}".format(kind, name)

def constraints_from_platform(platform):
    """Return the set of constraints from a platform name.

    Args:
        platform: The platform name.

    Returns:
        A list containing the constraints to satisfy for this platform.
    """

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
