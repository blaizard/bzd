---
name: bzd-documentation
description: How to write, update, and wire up Markdown documentation in the bzd monorepo using @bzd_rules_doc
compatibility: opencode
---

- All docs are Markdown (`.md`), wired into Bazel via `@bzd_rules_doc//doc:defs.bzl` and aggregated by the `doc_binary` in `docs/BUILD.bazel`.
- Place a module's docs next to its code (`README.md`, or a `docs/` subfolder if several files).
- Audience: an engineer who wants to _use_ the code. Keep each page ~2min read, use short sections, mermaid diagrams, and minimal internals.
- **Top-level component docs**: after the short description, add a "Key Points" section with a short list (max 5 bullets) of the component's main advantages/selling arguments (e.g. compile-time validation, zero allocation, extensibility).
- **Updating existing docs**: do not rewrite; only fix what is wrong (incorrect/outdated content, grammar, typos) and leave the rest untouched.

Always expose a module's docs with `doc_binary` (never `doc_library`), always named `documentation`. This way the user can build and visualize only this documentation when needed:

```
./tools/bazel run //path/to/module:documentation
```

Recreate the directory path in `srcs`: each module includes its own markdown files plus an entry per sub-directory that has docs, linking to that sub-directory's `documentation` `doc_binary` (titled with the directory name). Every parent directory along the path does the same, rebuilding the hierarchy level by level. Only the top-level modules are linked into `<root>/docs/BUILD.bazel`.

Module's `BUILD.bazel`:

```python
load("@bzd_rules_doc//doc:defs.bzl", "doc_binary")

doc_binary(
    name = "documentation",
    srcs = [
        ("Title", "README.md"),                        # a markdown file
        ("Sub Directory", "//path/to/sub:documentation"),  # a child doc_binary, recreates the directory path
    ],
    visibility = ["//visibility:public"],              # so the parent directory's doc_binary can link it
)
```

Then only the top-level modules are added to the master `docs/BUILD.bazel`:

```python
doc_binary(
    name = "docs",
    srcs = [
        ("**", "//:documentation"),
        ("New Top-Level Module", "//new/module:documentation"),
    ],
)
```

Notes:

- `doc_cc_library` generates Doxygen-based API reference from C++ headers.
- `bdl_system_diagram` (`@bzd_bdl//diagram:defs.bzl`) renders a BDL system diagram; add it to `deps` and reference in Markdown with `::: //path:target`.

Verify: `./tools/bazel run //docs` (build/preview) and `./tools/bazel run //:sanitizer` (markdown formatting).
