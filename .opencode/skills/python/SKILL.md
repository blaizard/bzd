---
name: python
description: How to run, test, build, type-check, format, and manage Python dependencies in the bzd monorepo
compatibility: opencode
---

## Overview

- **Python version**: 3.12 everywhere (toolchain, pip hubs, mypy, type stubs)
- **Build system**: Bazel with `rules_python` (hermetic interpreter — never invoke system `python3` or `pip` directly)
- **Type checker**: mypy (run via Bazel aspect `--config=mypy`)
- **Formatter/Linter**: ruff (run via sanitizer)
- **Test framework**: Python's built-in `unittest` (no pytest)
- **Naming convention**: `camelCase` for functions/variables (repo-wide convention, not standard Python `snake_case`)

---

## Project Structure

| Path                            | Purpose                                                                            |
| ------------------------------- | ---------------------------------------------------------------------------------- |
| `python/bzd/`                   | Core `bzd_python` library module (parsers, utils, HTTP, logging, validation, etc.) |
| `python/MODULE.bazel`           | Declares the `bzd_python` Bazel module                                             |
| `python/defs.bzl`               | Public API re-exports for custom Python Bazel rules                                |
| `python/requirements.in`        | Direct pip deps for `bzd_python` library — edit this to add deps                   |
| `python/requirements.txt`       | Locked pip deps for `bzd_python` (auto-generated, do NOT edit manually)            |
| `tools/python/requirements.in`  | Direct pip deps for root workspace apps/tools — edit this to add deps              |
| `tools/python/requirements.txt` | Locked pip deps for root workspace (auto-generated, do NOT edit manually)          |
| `tools/python/pyproject.toml`   | Config for mypy, yapf, ruff (Python 3.12, tabs, 120-col limit)                     |
| `tools/python/BUILD.bazel`      | Bazel targets for mypy, ruff, yapf, codespell binaries                             |

### Two pip hubs

| Hub name         | `requirements.in`              | Used in BUILD files via                                     |
| ---------------- | ------------------------------ | ----------------------------------------------------------- |
| `bzd_python_pip` | `python/requirements.in`       | `load("@bzd_python_pip//:requirements.bzl", "requirement")` |
| `pip`            | `tools/python/requirements.in` | `load("@pip//:requirements.bzl", "requirement")`            |

---

## Running, Testing, and Building

```bash
# Run all python tests in the bzd_python library module
./tools/bazel test @bzd_python//...

# Run a specific test target
./tools/bazel test @bzd_python//bzd/utils/tests:dict
./tools/bazel test @bzd_python//bzd/parser/tests:grammar

# Build a py_binary
./tools/bazel build @bzd_python//bzd/apps/map_analyzer:map_analyzer

# Run a py_binary
./tools/bazel run @bzd_python//bzd/apps/map_analyzer:map_analyzer

# Run all Python tests in the root workspace
./tools/bazel test //apps/...
```

---

## Type Checking with mypy

mypy runs as a Bazel build **aspect** — not a standalone command.

```bash
# Run mypy on the entire bzd_python library module
./tools/bazel build --config=mypy @bzd_python//...

# Run mypy on all root workspace targets
./tools/bazel build --config=mypy //...

# Run mypy on a single target
./tools/bazel build --config=mypy @bzd_python//bzd/utils:run
```

**mypy settings** (from `tools/python/pyproject.toml`):

- `python_version = "3.12"`
- Invoked with `--strict --explicit-package-bases --pretty --show-error-context`
- Third-party pip package deps are automatically silenced (`follow_imports = silent`)
- To skip mypy on a specific target, add the tag `"mypy-ignore"` in its `BUILD.bazel` entry

**Type hints are mandatory** on all function signatures.

---

## Formatting and Linting

```bash
# Run sanitizer on changed files (formats + lints, includes ruff for Python)
./tools/bazel run //:sanitizer

# Run on ALL files in the repo
./tools/bazel run //:sanitizer -- --all

# Check-only mode (reports issues, no writes)
./tools/bazel run //:sanitizer -- --check --all
```

**Style rules** (from `tools/python/pyproject.toml`):

- **Indentation**: tabs (not spaces)
- **Column limit**: 120 characters
- **Style**: PEP8-based (ruff enforces this)

To exclude a file or directory from sanitizer checks, place a `.sanitizerignore` file in that directory.

---

## Adding a New Internal Python Library

1. Create the `.py` source file(s):
   - Core shared library → `python/bzd/<module>/my_lib.py`
   - App-specific module → `apps/<app_name>/my_module.py`

2. Create or update the `BUILD.bazel`:

```python
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "my_lib",
    srcs = ["my_lib.py"],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",  # example internal dep
    ],
)
```

3. Import paths follow the directory structure from the workspace root:
   - File at `python/bzd/utils/my_lib.py` → `from bzd.utils.my_lib import MyClass`
   - File at `apps/my_app/my_module.py` → `from apps.my_app.my_module import MyClass`

4. Reference in other BUILD files:
   - Within `python/` (same module): `"//bzd/utils:my_lib"`
   - From root workspace targeting `bzd_python`: `"@bzd_python//bzd/utils:my_lib"`
   - From `apps/` targeting another app: `"//apps/my_app:my_module"`

---

## Adding New 3rd Party Python Dependencies

### For `python/bzd/` library code (hub: `bzd_python_pip`)

1. Add the package name to `python/requirements.in`
2. Regenerate the lockfile:
   ```bash
   ./tools/bazel run @bzd_python//:requirements.update
   ```
3. Use in `BUILD.bazel`:

   ```python
   load("@bzd_python_pip//:requirements.bzl", "requirement")

   py_library(
       name = "my_lib",
       srcs = ["my_lib.py"],
       deps = [requirement("paramiko")],
   )
   ```

### For `apps/` or `tools/` code (hub: `pip`)

1. Add the package name to `tools/python/requirements.in`
2. Regenerate the lockfile:
   ```bash
   ./tools/bazel run //tools/python:requirements.update
   ```
3. Use in `BUILD.bazel`:

   ```python
   load("@pip//:requirements.bzl", "requirement")

   py_binary(
       name = "my_app",
       srcs = ["my_app.py"],
       deps = [requirement("pillow")],
   )
   ```

---

## Writing Python Tests

All tests use Python's built-in `unittest`. Test file naming convention: `<module>_test.py`.

```python
# my_feature_test.py
import unittest

class TestMyFeature(unittest.TestCase):
    def testSomething(self) -> None:
        self.assertEqual(1 + 1, 2)

if __name__ == "__main__":
    unittest.main()
```

```python
# BUILD.bazel
load("@rules_python//python:defs.bzl", "py_test")

py_test(
    name = "my_feature",
    srcs = ["my_feature_test.py"],
    main = "my_feature_test.py",
    visibility = ["//visibility:public"],
    deps = ["//my_lib:my_feature"],
)
```

### Tests with data files

Use `args` with `$(location ...)` and `data` to pass file paths to the test:

```python
py_test(
    name = "parser",
    srcs = ["parser_test.py"],
    args = [
        "$(location testdata.json)",
        "$(location testdata2.json)",
    ],
    data = [
        "testdata.json",
        "testdata2.json",
    ],
    deps = [":parser"],
)
```

In the test file, read paths from `sys.argv`:

```python
import sys, pathlib, unittest

class TestParser(unittest.TestCase):
    dataFile: pathlib.Path

if __name__ == "__main__":
    TestParser.dataFile = pathlib.Path(sys.argv.pop(1))
    unittest.main()
```

### Bulk test generation (for many small test files)

```python
[py_test(
    name = path.replace(".py", ""),
    srcs = [path],
    deps = [":my_lib"],
) for path in glob(["*_test.py"])]
```

---

## Custom Bazel Rules for Python

All custom rules are in `python/private/` and exported via `python/defs.bzl`.

### `bzd_python_binary_library`

Exposes Bazel-built executables as importable Python path constants:

```python
load("@bzd_python//:defs.bzl", "bzd_python_binary_library")

bzd_python_binary_library(
    name = "my_binaries",
    executables = {"//tools/my_tool": "my_tool"},
)
```

### `bzd_python_hermetic_launcher`

Wraps a `py_binary` to use the hermetic Bazel Python interpreter instead of the system one:

```python
load("@bzd_python//:defs.bzl", "bzd_python_hermetic_launcher")

bzd_python_hermetic_launcher(
    name = "my_app.hermetic",
    binary = ":my_app",
)
```

### `bzd_python_oci`

Builds a Docker OCI image from a `py_binary` (automatically applies the hermetic launcher):

```python
load("@bzd_python//:defs.bzl", "bzd_python_oci")

bzd_python_oci(
    name = "image",
    binary = ":my_app",
    base = "@oci_base_image",
)
# Produces: :image (OCI artifact), :image.load (for `docker load`)
```

---

## Code Style Rules

- **Indentation**: tabs (not spaces) — enforced by ruff
- **Column limit**: 120 characters
- **Type hints**: mandatory on all function signatures (`from typing import ...`)
- **Docstrings**: Google-style (`Args:`, `Returns:`)
- **Naming**: `camelCase` for functions and variables (repo-wide convention)
- **Classes/Types**: `PascalCase`
- **Private members**: append underscore (e.g., `myHelper_()`, `value_`)
- **No `print()`**: use `bzd.logging` (`from bzd.logging import Logger`)
- **No dynamic imports or `importlib`**: declare all deps in `BUILD.bazel`
- **No `#bzd/` import prefix**: that pattern applies only to Node.js (`.mjs`) files

---

## CI / Quality Gate

```bash
# Full quality gate (runs everything: tests, mypy, sanitizer)
./quality_gate.sh

# Static analysis only (mypy aspect on all targets)
./tools/bazel build --config=mypy //...

# Sanitizer check only (ruff + codespell)
./tools/bazel run //:sanitizer -- --check --all
```

The CI pipeline runs `@bzd_python//...` in the test stage and `--config=mypy` in the static analysis stage. Always run the sanitizer before committing.
