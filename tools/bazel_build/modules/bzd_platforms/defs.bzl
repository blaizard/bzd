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

_AL_CONSTRAINTS_SYNONYMS = {
}

_ISA_CONSTRAINTS_SYNONYMS = {
    "arm64": ("aarch64",),
    "x86_64": ("x86-64", "amd64"),
}

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

    for name in ISA_CONSTRAINTS.keys():
        if isa_cleaned == name or isa_cleaned in _ISA_CONSTRAINTS_SYNONYMS.get(name, {}):
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

    for name in AL_CONSTRAINTS.keys():
        if al_cleaned == name or al_cleaned in _AL_CONSTRAINTS_SYNONYMS.get(name, {}):
            return name
    return None

def get_repository_platform(repository_ctx):
    """Get the current platform from a repository context

    Args:
        repository_ctx: The repository context.

    Returns:
        A string corresponding to the current platform or None if no match.
    """

    maybe_al = to_al(repository_ctx.os.name)
    maybe_isa = to_isa(repository_ctx.os.arch)
    if maybe_isa and maybe_al:
        return maybe_al + "-" + maybe_isa
    return None
