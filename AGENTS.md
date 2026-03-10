# AGENTS.md

This file provides essential information for agentic coding agents (like yourself) working in the `bzd` repository.

## Overview

The `bzd` project is a polyglot monorepo (C++, Python, Node.js, Rust) focused on lightweight, flexible IoT applications. The core philosophy centers on memory safety, zero dynamic allocation, and cross-platform compatibility (from 8-bit MCUs to x86_64).

## Build, Test, and Code Quality

The project uses Bazel for building and testing. Use the provided `./tools/bazel` wrapper to ensure the correct environment and toolchains are used.

### Common Commands

- **Build/Test everything:** `./tools/bazel test //...`
- **Run a single test:** `./tools/bazel test //cc/bzd/core/async/tests:async`
- **Run all static sanity checks (sanitizer):** `./tools/bazel run //:sanitizer`
- **Run full quality gate:** `./quality_gate.sh`
- **Build documentation:** `./tools/bazel run //docs`
- **Update compile_commands.json:** `./tools/bazel run @bzd_sanitizer//aspects/compile_commands:build`
- **Update rust-project.json:** `./tools/bazel run @rules_rust//tools/rust_analyzer:gen_rust_project -- //rust/...`

### Build Configurations

- `--config=debug`: Enable debug symbols and environment.
- `--config=prod`: Optimized production builds.
- `--config=remote`: Build on remote execution servers.
- `--config=docker`: Run toolchains locally in a docker sandbox.
- `--config=asan|lsan|tsan|ubsan`: Enable various sanitizers.

### Common Target Patterns

- `//...`: All targets in the repository.
- `//cc/...`: All C++ targets.
- `//python/...`: All Python targets.
- `//nodejs/...`: All Node.js targets.
- `//rust/...`: All Rust targets.
- `//targets/posix:main`: A specific executable for the POSIX platform.

### Toolchain Management

The project manages its own toolchains via Bazel. Do NOT install global compilers or libraries unless explicitly instructed.

- C++: Clang/GCC (managed via `bzd_toolchain_cc`).
- Rust: Hermetic (managed via `bzd_toolchain_rust`).
- Python: Hermetic Python interpreter.
- Node.js: Hermetic Node.js and npm/pnpm.

## Code Style Guidelines

### General (Cross-Language)

- **File/Directory Naming:** Lowercase `snake_case` (regex: `[a-z0-9_/.]`). This is strictly enforced.
- **Indentation:** Tabs are used for all files and all languages (unless unsupported by the language).
- **Naming Conventions:**
  - **Variables/Constants/Functions/Namespaces:** `camelCase`.
  - **Types (Classes/Structs/Enums/Traits):** `PascalCase`.
  - **Non-public members:** Append an underscore (e.g., `member_`, `privateFunction_()`).
  - **Abbreviations:** Avoid; prefer descriptive and meaningful names.

### C++ Specifics

- **Extensions:** `.cc` for implementation, `.hh` or `.h` for headers.
- **Braces:** Allman style (opening brace on a new line).
- **Namespaces:** Wrap core code in `namespace bzd { ... }`.
- **Memory:** NO dynamic allocations (`new`, `malloc`). Use fixed-size containers from `bzd::container`.
- **Types:** Use `bzd` specific types instead of standard or built-in ones:
  - `bzd::Size`, `bzd::Bool`, `bzd::Int32`, `bzd::UInt8`, `bzd::Float32`, etc.
- **Error Handling:** Use `bzd::Result<T, E>`.
  - Return `bzd::error::make(ErrorType::failure, "message")` for errors.
  - Return `bzd::nullresult` for success in `bzd::Result<void>`.
  - Example: `return bzd::error::make(ErrorType::failure, "Operation failed");`
- **Async:** Built on C++20 coroutines.
  - Return type: `bzd::Async<T>`.
  - Use `co_await` for awaiting promises and `co_return` for returning values.
- **Attributes:** Always use `[[nodiscard]]` for functions that return a value.
- **Comments:** Add code comments sparingly. Focus on _why_ something is done, especially for complex logic, rather than _what_ is done. Only add high-value comments if necessary for clarity or if requested by the user. Do not edit comments that are separate from the code you are changing. _NEVER_ talk to the user or describe your changes through comments.

### Python Specifics

- **Extension:** `.py`.
- **Naming:** Follows the general `camelCase` for functions and variables.
- **Types:** Type hints are mandatory for all function signatures. Use `from typing import ...`.
- **Linting/Formatting:** Uses `mypy` for type checking and `yapf` for formatting.
- **Docstrings:** Use Google-style docstrings (Args, Returns).

### Node.js Specifics

- **Extension:** `.mjs` (ES Modules are preferred).
- **Naming:** Follows the general `camelCase` for functions and variables.
- **Imports:** Use the `#bzd/` prefix for internal project imports to resolve against the monorepo root.
  - Example: `import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";`
- **Linting/Formatting:** Uses `eslint` and `prettier`.

### Rust Specifics

- **Extension:** `.rs`.
- **Naming:** Follow the project's `camelCase` convention for functions/variables where possible for consistency, although standard Rust `snake_case` is sometimes encountered.
- **Core:** Core libraries often use `#![no_std]`.

## Project Structure

- `cc/`: C++ core library, containers, and platform abstractions.
- `python/`: Python utilities, build scripts, and tools.
- `nodejs/`: Node.js utilities and web-based components (Vue).
- `rust/`: Rust implementation of specific components and targets.
- `tools/`: Bazel wrappers, CI scripts, and repository-wide tools.
- `targets/`: Platform-specific code and entry points (ESP32, POSIX, Linux, etc.).
- `interfaces/`: Definition of hardware and software interfaces.
- `docs/`: Project documentation and assets.

## Contribution Rules

- **Sanitizer:** Always run `bazel run //:sanitizer` before committing. It formats code and checks structure.
- **Memory Safety:** NEVER introduce dynamic memory allocations in the C++ core.
- **Dependencies:** Prefer internal `bzd` core components over external libraries or the standard template library (STL).
- **Quality Gate:** For significant changes, ensure `./quality_gate.sh` passes locally.
- **Commit Messages:** Follow the project's style (typically concise, describing "why").

## Git Safety Protocol

- NEVER update the git config.
- NEVER run destructive/irreversible git commands unless explicitly requested.
- Avoid `git commit --amend` unless you created the commit in this session and it hasn't been pushed.
- Use `gh` for GitHub-related tasks (PRs, issues).

### Other Tools

- **Skylark/Bazel files:** Formatted with `buildifier`.
- **Markdown/YAML/JSON:** Supported by the sanitizer for formatting and linting.
- **Doxygen:** Used for C++ documentation generation (triggered via `bazel run //docs`).

## Operational Guidelines for Agents

- **Path Construction:** Always use absolute paths for file system tools.
- **Safety:** Before executing commands that modify the file system or system state, provide a brief explanation.
- **Conciseness:** Be professional and direct in CLI interactions. Avoid conversational filler.
- **Linter Errors:** If you encounter LSP errors during file operations, analyze them but prioritize fulfilling the user's primary request unless the errors block your progress.
- **Verification:** Always verify changes by running the relevant Bazel test or the sanitizer.
