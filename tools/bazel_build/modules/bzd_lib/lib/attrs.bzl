"""Attributes relates rules."""

ATTRS_COMMON_BUILD_RULES = {
    "compatible_with": 1,
    "deprecation": 1,
    "distribs": 1,
    "exec_compatible_with": 1,
    "exec_properties": 1,
    "features": 1,
    "restricted_to": 1,
    "tags": 1,
    "target_compatible_with": 1,
    "testonly": 1,
    "toolchains": 1,
    "visibility": 1,
}

ATTRS_COMMON_TEST_RULES = {
    "args": 1,
    "env": 1,
    "env_inherit": 1,
    "flaky": 1,
    "local": 1,
    "shard_count": 1,
    "size": 1,
    "timeout": 1,
}

ATTRS_COMMON_BINARY_RULES = {
    "args": 1,
    "env": 1,
    "output_licenses": 1,
}

def attrs_assert_any_of(kwargs, *attr_collections):
    """Assert that the arguments provided are valid.

    Args:
        kwargs: The key arguments to be checked.
        *attr_collections: One or more collection of attributes.
    """

    for name in dict(**kwargs).keys():
        if not any([name in collection for collection in attr_collections]):
            fail("The attribute name '{}' is not a supported by this macro.".format(name))
