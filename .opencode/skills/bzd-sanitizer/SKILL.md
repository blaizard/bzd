---
name: bzd-sanitizer
description: How to run the sanitizer to format and lint code in the bzd monorepo
compatibility: opencode
---

## Overview

The sanitizer formats code and checks structure across all languages (C++, Python, Node.js, Rust, Bazel).

---

## Usage

```bash
./tools/bazel run //:sanitizer
```

This formats everything that is not staged. Always use this command.

Do **not** run it with `--check`, which only reports issues without modifying files; it is useful for CI only.

---

## After Sanitizing

After the sanitizer modifies files, re-run the affected tests:

```bash
./tools/bazel test //path/to/affected/...
```
