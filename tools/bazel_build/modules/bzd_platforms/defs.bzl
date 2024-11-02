"""Platform helpers."""

load("//:constraints.bzl", "AL_CONSTRAINTS", "ISA_CONSTRAINTS")

def _constraint_target(kind, name):
    """Create the constraint target string.

    Args:
        kind: The kind of constraint.
        name: The name of the constraint.

    Returns:
        A string corresponding to the constraint target.
    """

    return Label("//{}:{}".format(kind, name))

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

def to_isa(isa):
    """Convert the given string into a known isa.

    Args:
        isa: A string containing the ISA.

    Returns:
        The ISA or None if it cannot be matched.
    """

    isa_cleaned = isa.lower().strip()

    for name, constraint in ISA_CONSTRAINTS.items():
        if isa_cleaned == name or isa_cleaned in constraint.get("synonyms", {}):
            return name
    return None

def to_al(al):
    """Convert the given string into a known al.

    Args:
        al: A string containing the AL.

    Returns:
        The AL or None if it cannot be matched.
    """

    al_cleaned = al.lower().strip()

    for name, constraint in AL_CONSTRAINTS.items():
        if al_cleaned == name or al_cleaned in constraint.get("synonyms", {}):
            return name
    return None

def get_platform_from_os(os):
    """Get the current platform from a repository_os

    Args:
        os: The repository os context.

    Returns:
        A string corresponding to the current platform or None if no match.
    """

    maybe_al = to_al(os.name)
    maybe_isa = to_isa(os.arch)
    if maybe_isa and maybe_al:
        return maybe_al + "-" + maybe_isa
    return None
