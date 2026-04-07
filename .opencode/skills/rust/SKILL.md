---
name: rust
description: How to write, build, test, format, and contribute Rust code in the bzd monorepo
compatibility: opencode
---

## Overview

- **Build system**: Bazel via `./tools/bazel` — use `//rust:defs.bzl` custom macros, never raw `rules_rust` rules directly for binaries/tests
- **Toolchain**: Hermetic Rust (managed by `bzd_toolchain_rust` / `rules_rust`) — **never invoke `cargo` or `rustc` directly**
- **`#![no_std]`**: Core library code uses `#![no_std]` for embedded compatibility — avoid `std`-only APIs in library crates
- **`#![no_main]`**: Library/test crates that use `bzd_test` must declare `#![no_main]`
- **Test framework**: Custom `bzd_test` proc-macro (NOT the standard libtest harness — `use_libtest_harness` is always `false`)
- **Async runtime**: Embassy (via `rust_embassy_test` or `//rust/targets:embassy`) for async tasks on std and bare-metal
- **Static analysis**: Clippy via `--config=clippy`
- **Naming convention**: `snake_case` for functions and variables (standard Rust convention — the sole exception to the repo-wide `camelCase` rule)

---

## Project Structure

| Path                      | Purpose                                                                               |
| ------------------------- | ------------------------------------------------------------------------------------- |
| `rust/defs.bzl`           | Custom Bazel macros: `rust_binary`, `rust_test`, `rust_embassy_test`                  |
| `rust/bzd/`               | Core `#![no_std]` library (`bzd::println!`, `bzd::exit!`, platform abstractions)      |
| `rust/bzd/base/`          | Low-level base types and panic handler                                                |
| `rust/libs/bzd_test/`     | Custom test framework (proc-macro + `#![no_std]` runtime — works on embedded and std) |
| `rust/libs/std/`          | Standard library group (`//rust/libs/std`) for linux-only targets needing `std`       |
| `rust/targets/`           | Platform target aliases: `//rust/targets:main` and `//rust/targets:embassy`           |
| `rust/targets/std/`       | Linux/macOS std target implementations                                                |
| `rust/targets/esp/`       | ESP32 / ESP32S3 target implementations                                                |
| `rust/grep/`              | Reference example: binary + library + inline tests                                    |
| `rust/embassy/hello/`     | Reference example: Embassy async task (cross-platform: linux, ESP32, ESP32S3)         |
| `rust/adventofcode_2025/` | Additional example binaries                                                           |

---

## Running, Testing, and Building

```bash
# Build all Rust targets
./tools/bazel build //rust/...

# Test all Rust targets
./tools/bazel test //rust/...

# Test a specific target
./tools/bazel test //rust/grep:lib_test

# Show full test output even on success
./tools/bazel test --test_output=all //rust/grep:lib_test

# Run a binary
./tools/bazel run //rust/grep:grep

# Debug build
./tools/bazel test --config=debug //rust/grep:lib_test

# Update rust-project.json for IDE rust-analyzer support
./tools/bazel run @rules_rust//tools/rust_analyzer:gen_rust_project -- //rust/...
```

---

## Bazel Rules

### Custom bzd rules (for new binaries and tests)

```python
load("//rust:defs.bzl", "rust_binary", "rust_test", "rust_embassy_test")
```

| Rule                | Purpose                                                                                                  |
| ------------------- | -------------------------------------------------------------------------------------------------------- |
| `rust_binary`       | Wraps `rules_rust` `rust_binary` with the `bzd_runner`; use for all runnable binaries                    |
| `rust_test`         | Wraps `rust_test`; auto-injects `//rust/targets:main` + `//rust/libs/bzd_test`; disables libtest harness |
| `rust_embassy_test` | Like `rust_test` but injects `//rust/targets:embassy` (Embassy async executor)                           |

### Plain `rust_library` (for libraries)

```python
load("@rules_rust//rust:defs.bzl", "rust_library")
```

Use `rust_library` directly for all library targets — no custom wrapper needed.

**Key constraints:**

- Never set `use_libtest_harness = True` — the custom macros disable it automatically.
- Never add `//rust/targets:main` or `//rust/libs/bzd_test` to `deps` manually — `rust_test` injects them.
- `rust_test` and `rust_embassy_test` accept a `crate` attribute to test an existing `rust_library` target inline.

---

## Code Style

- **Indentation**: tabs (not spaces) — repo-wide rule, enforced by the sanitizer
- **Column limit**: 120 characters
- **Naming**: `snake_case` for functions and variables (standard Rust convention)
- **Types** (structs, enums, traits): `PascalCase`
- **`#![no_std]`**: required on all core library crates; avoid `std`-only imports
- **`#![no_main]`**: required on library/test crates using `bzd_test`
- **No `println!` / `eprintln!`**: use `bzd::println!` (from `//rust/bzd`) on embedded targets
- **No `panic!` for errors**: return `Result` types; use `?` for propagation
- **`#[allow(...)]`**: only use when Clippy produces a false positive — add a comment explaining why

### Example library

```rust
#![no_std]
#![no_main]

pub fn search<'a>(query: &'a str, contents: &'a str) -> impl Iterator<Item = &'a str> {
	contents.lines().filter(move |line| line.contains(query))
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
	use super::*;

	#[test]
	fn test_search() -> TestResult {
		let result = search("duct", "a duct tape\nanother line");
		assert_eq!(result.collect::<Vec<_>>(), vec!["a duct tape"])?;
		Ok(())
	}
}
```

---

## Adding a New Library

1. Create `rust/<module>/lib.rs`:

```rust
#![no_std]
#![no_main]

pub fn my_function(input: &str) -> bool {
	!input.is_empty()
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
	use super::*;

	#[test]
	fn test_my_function() -> TestResult {
		assert_eq!(my_function("hello"), true)?;
		assert_eq!(my_function(""), false)?;
		Ok(())
	}
}
```

2. Create `rust/<module>/BUILD.bazel`:

```python
load("@rules_rust//rust:defs.bzl", "rust_library")
load("//rust:defs.bzl", "rust_test")

rust_library(
	name = "my_module",
	srcs = ["lib.rs"],
	visibility = ["//visibility:public"],
	deps = [
		# Add deps here, e.g. "//rust/libs/std" for std targets
	],
)

rust_test(
	name = "my_module_test",
	crate = ":my_module",
)
```

3. Reference from another target's BUILD file:

```python
deps = ["//rust/my_module:my_module"]
```

---

## Writing Tests

Tests use the custom `bzd_test` proc-macro framework. This framework works on all platforms (`#![no_std]` compatible).

### Inline tests (preferred — tests live in the same file as the code)

```rust
#![no_main]

pub fn add(a: i32, b: i32) -> i32 {
	a + b
}

#[cfg(test)]
#[bzd_test::test]
mod tests {
	use super::*;

	#[test]
	fn test_add() -> TestResult {
		assert_eq!(add(1, 2), 3)?;
		Ok(())
	}

	#[test]
	fn test_add_negative() -> TestResult {
		assert_eq!(add(-1, -2), -3)?;
		Ok(())
	}

	#[test]
	#[ignore]
	fn test_skipped() -> TestResult {
		// This test is skipped at runtime
		Ok(())
	}
}
```

**Key points:**

- `#[bzd_test::test]` on the `mod tests` block (NOT on individual `#[test]` functions).
- Return type is `TestResult` (`Result<(), TestError>`) — automatically in scope inside the `#[bzd_test::test]` block.
- `assert_eq!(a, b)?` — custom macro from `bzd_test`; **must use `?`** to propagate the assertion failure.
- `#[ignore]` skips the test case at runtime (it is registered but not executed).
- `#![no_main]` is required at crate root when using `bzd_test`.

### Separate test file (for a crate-level test)

```rust
// rust/<module>/tests/lib.rs
#![no_std]
#![no_main]

#[cfg(test)]
#[bzd_test::test]
mod tests {
	#[test]
	fn test_something() -> TestResult {
		assert_eq!(1 + 1, 2)?;
		Ok(())
	}
}
```

### Test BUILD.bazel patterns

**Inline tests (test the library crate directly):**

```python
load("@rules_rust//rust:defs.bzl", "rust_library")
load("//rust:defs.bzl", "rust_test")

rust_library(
	name = "lib",
	srcs = ["lib.rs"],
	visibility = ["//visibility:public"],
)

rust_test(
	name = "lib_test",
	crate = ":lib",
)
```

**Separate test crate:**

```python
load("@rules_rust//rust:defs.bzl", "rust_library")
load("//rust:defs.bzl", "rust_test")

rust_library(
	name = "tests",
	srcs = ["tests/lib.rs"],
)

rust_test(
	name = "test_crate",
	crate = ":tests",
)
```

**Embassy async test:**

```python
load("@rules_rust//rust:defs.bzl", "rust_library")
load("//rust:defs.bzl", "rust_embassy_test")

rust_library(
	name = "lib",
	srcs = ["lib.rs"],
	deps = ["//rust/targets:embassy"],
)

rust_embassy_test(
	name = "lib_test",
	crate = ":lib",
)
```

---

## Async / Embassy

Embassy is the async runtime used for both bare-metal (ESP32) and std (Linux/macOS) targets.

```rust
#![cfg_attr(target_os = "none", no_std)]
#![no_main]

// This import brings in the platform bootstrap (panic handler, executor setup).
use embassy as _;

use embassy_executor::Spawner;

#[embassy_executor::task]
async fn my_task() {
	// async work here
	bzd::exit!(0);
}

#[unsafe(no_mangle)]
pub fn run_embassy(spawner: Spawner) {
	spawner.spawn(my_task()).unwrap();
}
```

**Key notes:**

- `use embassy as _` pulls in the platform bootstrap (provides `main` on std, panic handler on bare-metal).
- `#[unsafe(no_mangle)] pub fn run_embassy(spawner: Spawner)` is the mandatory entry point name.
- `bzd::println!` and `bzd::exit!` (from `//rust/bzd`) work on all platforms.
- Use `rust_embassy_test` (not `rust_test`) when testing embassy-based crates.
- `#![cfg_attr(target_os = "none", no_std)]` keeps the crate `#![no_std]` on embedded only.

---

## Platforms and Targets

| Platform constraint                         | `//rust/targets:main` | `//rust/targets:embassy` | Notes              |
| ------------------------------------------- | --------------------- | ------------------------ | ------------------ |
| `@bzd_platforms//al:linux`                  | ✓                     | ✓                        | Linux x86_64       |
| `@bzd_platforms//al:osx`                    | ✓                     | ✓                        | macOS ARM64        |
| `@bzd_platforms//al_isa:esp32-xtensa_lx6`   | ✓                     | ✓                        | ESP32 (Xtensa LX6) |
| `@bzd_platforms//al_isa:esp32s3-xtensa_lx7` | ✓                     | ✓                        | ESP32-S3 (LX7)     |

**Platform-constrained test** (linux-only):

```python
rust_test(
	name = "linux_test",
	crate = ":lib",
	target_compatible_with = ["@bzd_platforms//al:linux"],
)
```

**Conditional deps by platform:**

```python
rust_library(
	name = "lib",
	srcs = ["lib.rs"],
	deps = ["//rust/bzd"] + select({
		"@bzd_platforms//al_isa:esp32-xtensa_lx6": ["@bzd_espressif//hal/rust:embassy"],
		"@bzd_platforms//al_isa:esp32s3-xtensa_lx7": ["@bzd_espressif//hal/rust:embassy"],
		"//conditions:default": ["@crates//:embassy-executor"],
	}),
)
```

Use `//rust/libs/std` when a library explicitly requires the Rust standard library (linux/osx only):

```python
deps = ["//rust/libs/std"]
```

---

## Formatting and Static Analysis

```bash
# Run the sanitizer (rustfmt + linting) on changed files
./tools/bazel run //:sanitizer

# Run on ALL files in the repo
./tools/bazel run //:sanitizer -- --all

# Check-only mode (report issues, no writes)
./tools/bazel run //:sanitizer -- --check --all

# Run Clippy static analysis on all Rust targets
./tools/bazel build --config=clippy //rust/...

# Run Clippy on a specific target
./tools/bazel build --config=clippy //rust/grep/...
```

To exclude a directory from sanitizer checks, place a `.sanitizerignore` file in that directory.

---

## CI / Quality Gate

```bash
# Full quality gate (tests + sanitizer + static analysis)
./quality_gate.sh

# Test all Rust targets
./tools/bazel test //rust/...

# Sanitizer check only
./tools/bazel run //:sanitizer -- --check --all

# Clippy check only
./tools/bazel build --config=clippy //rust/...
```

Always run `./tools/bazel run //:sanitizer` before committing.
