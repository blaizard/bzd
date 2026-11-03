"""Rust crate information helpers."""

load("@rules_rust//rust:defs.bzl", "rust_common")

def merge_crate_info(deps):
    """Merge rust crate providers into a single one.

    Args:
        deps: The list of dependencies to merge.

    Returns:
        The merged crate group information.
    """

    dep_infos = []
    dep_transitive_infos = []

    for dep in deps:
        if rust_common.crate_info in dep:
            dep_infos.append(
                rust_common.dep_variant_info(
                    crate_info = dep[rust_common.crate_info],
                    dep_info = dep[rust_common.dep_info],
                    cc_info = None,
                    build_info = None,
                ),
            )

        if rust_common.crate_group_info in dep:
            dep_transitive_infos.append(
                dep[rust_common.crate_group_info].dep_variant_infos,
            )

    return rust_common.crate_group_info(
        dep_variant_infos = depset(dep_infos, transitive = dep_transitive_infos),
    )
