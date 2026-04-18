---
name: sanitizer
description: How to run the sanitizer to format and lint code in the bzd monorepo
compatibility: opencode
---

## Overview

The sanitizer formats code and checks structure across all languages (C++, Python, Node.js, Rust, Bazel). Always run it before committing.

---

## Usage

### Sanitize non-staged (changed) files only (fastest)

```bash
./tools/bazel run //:sanitizer
```

This is the **fastest** option — only checks files that have been modified. Use this during development.

If there are no staged/staged files to sanitize, it will fall back to using the latest commit.

### Sanitize the entire repository

```bash
./tools/bazel run //:sanitizer -- --all
```

Use this when you need to sanitize everything (e.g., before a major commit or after pulling large changes).

### Check-only mode (don't modify files)

```bash
./tools/bazel run //:sanitizer -- --check
./tools/bazel run //:sanitizer -- --check --all
```

Useful for CI checks or to see what would change without actually modifying files.

### Sanitize a specific path

```bash
# Using --all with a path
./tools/bazel run //:sanitizer -- --all cc/bzd/core

# Or just the path (shorthand)
./tools/bazel run //:sanitizer -- cc/bzd/core
```

This sanitizes all files under the specified directory.

---

## Performance Tip

**Always prefer running without `--all`** whenever possible. The sanitizer operates on changed files only and is significantly faster:

- Without `--all`: ~seconds (checks git-tracked changes)
- With `--all`: ~minutes (checks entire repository)

---

## Excluding Directories

To exclude a directory from sanitizer checks, place a `.sanitizerignore` file in that directory.

---

## After Sanitizing

After the sanitizer modifies files, re-run the affected tests:

```bash
./tools/bazel test //path/to/affected/...
```
