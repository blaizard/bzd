---
name: bazel
description: How to build, test, run, and maintain Bazel targets in the bzd monorepo — always load this before invoking any ./tools/bazel command
compatibility: opencode
---

## Mandatory usage

Load this skill before invoking **any** `./tools/bazel` command. It governs:

- Which binary to invoke (always `./tools/bazel`, never system `bazel`)
- Target label syntax and common target patterns
- Build configs, test flags, and output options
- How to run the sanitizer and language-specific static analysis
- How to regenerate dependency lockfiles
- How to query the build graph

---

## The `./tools/bazel` wrapper

**Always** use `./tools/bazel` — never the system `bazel`, and never system compilers or interpreters directly.

The wrapper ensures:

- Correct Bazel version (pinned in `.bazelversion`)
- Hermetic toolchains (C++, Python, Rust, Node.js) — all managed by Bazel, not the host OS
- Correct Bazel flags for this repository

```bash
# CORRECT
./tools/bazel build //cc/...

# WRONG — never do these
bazel build //cc/...
python3 script.py
cargo test
npm install
g++ main.cc
```

---

## Target Label Syntax

| Pattern                       | Meaning                                                    |
| ----------------------------- | ---------------------------------------------------------- |
| `//path/to/pkg:target`        | Specific named target in a package                         |
| `//path/to/pkg`               | Default target (same name as the last directory component) |
| `//path/to/pkg/...`           | All targets recursively under a directory                  |
| `//...`                       | All targets in the entire repository                       |
| `@external_repo//path:target` | Target in an external Bazel module/repository              |

### Common language target roots

| Language | Root pattern       | Example                             |
| -------- | ------------------ | ----------------------------------- |
| C++      | `//cc/...`         | `//cc/bzd/container/tests:array`    |
| Python   | `@bzd_python//...` | `@bzd_python//bzd/utils/tests:dict` |
| Node.js  | `//nodejs/...`     | `//nodejs/core/tests:format`        |
| Rust     | `//rust/...`       | `//rust/grep:lib_test`              |
| Apps     | `//apps/...`       | `//apps/my_app:backend`             |

---

## Common Commands

### Building

```bash
# Build a specific target
./tools/bazel build //path/to:target

# Build all targets in a subtree
./tools/bazel build //cc/...

# Build everything
./tools/bazel build //...
```

### Testing

```bash
# Run a single test target
./tools/bazel test //path/to:target

# Run all tests under a directory (recursive)
./tools/bazel test //path/to/...

# Run all tests in the repo
./tools/bazel test //...

# Show full test output even on success
./tools/bazel test --test_output=all //path/to:target

# Show output only on failure (default is errors)
./tools/bazel test --test_output=errors //path/to:target

# Detailed test summary
./tools/bazel test --test_summary=detailed //path/to:target

# Re-run a test even if Bazel cached it
./tools/bazel test --cache_test_results=no //path/to:target
```

### Running

```bash
# Run a binary target
./tools/bazel run //path/to:target

# Run with arguments (-- separates Bazel flags from the binary's args)
./tools/bazel run //path/to:target -- --my-arg value
```

---

## Build Configurations (`--config=...`)

| Config       | Purpose                                               |
| ------------ | ----------------------------------------------------- |
| `debug`      | Debug symbols, debug environment (assertions enabled) |
| `prod`       | Optimized production build                            |
| `remote`     | Build on remote execution servers                     |
| `docker`     | Run toolchains locally inside a Docker sandbox        |
| `asan`       | Address Sanitizer (memory errors, use-after-free)     |
| `lsan`       | Leak Sanitizer (memory leaks)                         |
| `tsan`       | Thread Sanitizer (data races)                         |
| `ubsan`      | Undefined Behavior Sanitizer                          |
| `mypy`       | Run mypy type-checking aspect on Python targets       |
| `clang-tidy` | Run clang-tidy static analysis on C++ targets         |
| `clippy`     | Run Clippy static analysis on Rust targets            |

```bash
# Debug build
./tools/bazel test --config=debug //cc/bzd/core/async/tests:async

# Address Sanitizer
./tools/bazel test --config=asan //cc/...

# Thread Sanitizer on a specific test
./tools/bazel test --config=tsan //cc/bzd/core/async/tests:async
```

---

## Sanitizer (Formatter + Linter)

The sanitizer formats code and checks structure across all languages. Always run it before committing.

```bash
# Run on changed files only (fastest — use during development)
./tools/bazel run //:sanitizer

# Run on ALL files in the repository
./tools/bazel run //:sanitizer -- --all

# Check-only mode — report issues without modifying files
./tools/bazel run //:sanitizer -- --check --all

# Check only changed files without modifying
./tools/bazel run //:sanitizer -- --check
```

**After the sanitizer modifies files**, always re-run the affected tests to confirm nothing broke:

```bash
./tools/bazel test //path/to/affected/...
```

To exclude a directory from sanitizer checks, place a `.sanitizerignore` file in that directory.

---

## Language-Specific Static Analysis

Run static analysis after the sanitizer and before marking any task complete.

| Language    | Command                                                      |
| ----------- | ------------------------------------------------------------ |
| **Python**  | `./tools/bazel build --config=mypy <targets>`                |
| **C++**     | `./tools/bazel build --config=clang-tidy <targets>`          |
| **Rust**    | `./tools/bazel build --config=clippy <targets>`              |
| **Node.js** | Covered by the sanitizer (Prettier + ESLint) — no extra step |

```bash
# Python mypy on all targets
./tools/bazel build --config=mypy //...

# Python mypy on a single target
./tools/bazel build --config=mypy @bzd_python//bzd/utils:run

# C++ clang-tidy on all C++ targets
./tools/bazel build --config=clang-tidy //cc/...

# C++ clang-tidy on a specific subtree
./tools/bazel build --config=clang-tidy //cc/bzd/container/...

# Rust Clippy on all Rust targets
./tools/bazel build --config=clippy //rust/...

# Rust Clippy on a specific target
./tools/bazel build --config=clippy //rust/grep/...
```

---

## BUILD File Rules — Quick Reference

All new targets must use the appropriate **custom bzd rules**, not the raw upstream rules (except for libraries — see language skill for details).

| Language | Target type | Load statement                                         | Rule name            |
| -------- | ----------- | ------------------------------------------------------ | -------------------- |
| C++      | Library     | `load("//cc/bdl:cc.bzl", "bzd_cc_library")`            | `bzd_cc_library`     |
| C++      | Test        | `load("//cc/bdl:cc.bzl", "bzd_cc_test")`               | `bzd_cc_test`        |
| C++      | Binary      | `load("//cc/bdl:cc.bzl", "bzd_cc_binary")`             | `bzd_cc_binary`      |
| Python   | Library     | `load("@rules_python//python:defs.bzl", "py_library")` | `py_library`         |
| Python   | Test        | `load("@rules_python//python:defs.bzl", "py_test")`    | `py_test`            |
| Python   | Binary      | `load("@rules_python//python:defs.bzl", "py_binary")`  | `py_binary`          |
| Node.js  | Library     | `load("@bzd_rules_nodejs//nodejs:defs.bzl", ...)`      | `bzd_nodejs_library` |
| Node.js  | Test        | `load("@bzd_rules_nodejs//nodejs:defs.bzl", ...)`      | `bzd_nodejs_test`    |
| Node.js  | Binary      | `load("@bzd_rules_nodejs//nodejs:defs.bzl", ...)`      | `bzd_nodejs_binary`  |
| Rust     | Library     | `load("@rules_rust//rust:defs.bzl", "rust_library")`   | `rust_library`       |
| Rust     | Test        | `load("//rust:defs.bzl", "rust_test")`                 | `rust_test`          |
| Rust     | Binary      | `load("//rust:defs.bzl", "rust_binary")`               | `rust_binary`        |

See the language-specific skill for full BUILD file examples (`cc`, `python`, `nodejs`, `rust`).

---

## Dependency Lockfile Management

Never edit lockfiles manually. Always regenerate them via Bazel.

### Python — `bzd_python` library hub

```bash
# After editing python/requirements.in
./tools/bazel run @bzd_python//:requirements.update
```

### Python — root workspace hub

```bash
# After editing tools/python/requirements.in
./tools/bazel run //tools/python:requirements.update
```

### Node.js

```bash
# After editing tools/nodejs/requirements.in
./tools/bazel run //tools/nodejs:requirements
```

---

## IDE Tooling — Regenerating Index Files

These commands regenerate language-server index files for IDE support. Run them after adding or removing targets.

```bash
# C++ — regenerate compile_commands.json (for clangd)
./tools/bazel run @bzd_sanitizer//aspects/compile_commands:build

# Rust — regenerate rust-project.json (for rust-analyzer)
./tools/bazel run @rules_rust//tools/rust_analyzer:gen_rust_project -- //rust/...
```

---

## Querying the Build Graph

```bash
# List all direct and transitive deps of a target
./tools/bazel query 'deps(//path/to:target)'

# Find all targets that depend on a given target (reverse deps)
./tools/bazel query 'rdeps(//..., //path/to:target)'

# List all test targets under a directory
./tools/bazel query 'kind(.*_test, //path/to/...)'

# List all targets in a package
./tools/bazel query //path/to/pkg:all
```

---

## Full Quality Gate

Run this before any significant commit or PR:

```bash
# Runs tests, sanitizer, and static analysis end-to-end
./quality_gate.sh
```

Or run the individual stages manually:

```bash
# 1. All tests
./tools/bazel test //...

# 2. Sanitizer
./tools/bazel run //:sanitizer -- --check --all

# 3. Python type checking
./tools/bazel build --config=mypy //...

# 4. C++ static analysis
./tools/bazel build --config=clang-tidy //cc/...

# 5. Rust static analysis
./tools/bazel build --config=clippy //rust/...
```

---

## Key Constraints Checklist

Before running any Bazel command, confirm:

- [ ] I am using `./tools/bazel`, not system `bazel` or any system compiler/interpreter.
- [ ] The target label is correct (`//path/to/pkg:name` syntax).
- [ ] If I added a new source file, I have updated the `BUILD.bazel` first.
- [ ] If I added a new dependency, I have regenerated the appropriate lockfile.
- [ ] After the sanitizer modifies files, I re-run the affected tests.
- [ ] Static analysis passes (mypy / clang-tidy / clippy) before marking any task complete.
