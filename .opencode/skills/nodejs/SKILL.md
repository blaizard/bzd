---
name: nodejs
description: How to write, build, test, and contribute Node.js/JavaScript/Vue code in the bzd monorepo
compatibility: opencode
---

## Overview

- **Language**: JavaScript (ES Modules, `.mjs`) — no TypeScript, no `.js`, no `.cjs` for new code
- **Build system**: Bazel via `./tools/bazel` — use `@bzd_rules_nodejs//nodejs:defs.bzl` custom macros, never raw `nodejs_binary`
- **Node.js toolchain**: Hermetic (managed by `bzd_rules_nodejs`) — never invoke system `node` or `npm` directly
- **Package manager**: pnpm (managed automatically by Bazel install rules — never run `pnpm install` or `npm install` manually)
- **Test framework**: Mocha (run via Bazel, not directly)
- **Frontend**: Vue 3 + Vite (bundled via `bzd_nodejs_web_binary`)
- **Formatter**: Prettier (run via the sanitizer)
- **Import prefix**: `#bzd/` resolves to the monorepo root — always use this for cross-package imports
- **Naming convention**: `camelCase` for functions and variables (repo-wide convention)

---

## Project Structure

| Path                       | Purpose                                                                                    |
| -------------------------- | ------------------------------------------------------------------------------------------ |
| `nodejs/core/`             | Core runtime libraries: exception, log, format, cache, router, http, rest, websocket, etc. |
| `nodejs/core/tests/`       | Unit tests for core libraries (`*_test.mjs`)                                               |
| `nodejs/db/`               | Database abstractions: key-value store, storage, timeseries                                |
| `nodejs/email/`            | Email integration (SendGrid, etc.)                                                         |
| `nodejs/payment/`          | Payment integration (Stripe)                                                               |
| `nodejs/styles/`           | SCSS design tokens and themes                                                              |
| `nodejs/utils/`            | Utility functions: array, pathlib, regexpr, object, query, etc.                            |
| `nodejs/vue/`              | Vue 3 layer: apps, components, directives, router, plugins                                 |
| `nodejs/vue/apps/`         | Base frontend/backend app scaffolding (`frontend.mjs`, `backend.mjs`)                      |
| `nodejs/vue/apps/example/` | Minimal reference app (start here for a new app)                                           |
| `nodejs/vue/components/`   | Reusable Vue components: graph, layout, logger, menu, modal, terminal, etc.                |
| `nodejs/vue/directives/`   | Custom Vue directives: loading, resize, tooltip                                            |
| `tools/nodejs/`            | npm dependency lockfiles: `requirements.in`, `requirements.json`                           |
| `apps/`                    | Full application targets (each app has a `frontend/` and/or `backend/`)                    |

---

## Running, Testing, and Building

```bash
# Test all Node.js targets
./tools/bazel test //nodejs/...

# Test a specific target
./tools/bazel test //nodejs/core/tests:format
./tools/bazel test //nodejs/core/tests:router

# Show full test output even on success
./tools/bazel test --test_output=all //nodejs/core/tests:format

# Build a backend binary
./tools/bazel build //nodejs/vue/apps/example:backend

# Build a frontend bundle (produces a .bundle directory)
./tools/bazel build //nodejs/vue/apps/example:frontend

# Run a backend server
./tools/bazel run //nodejs/vue/apps/example:backend

# Test all app targets
./tools/bazel test //apps/...
```

---

## Bazel Rules

All Node.js targets use custom rules from `@bzd_rules_nodejs//nodejs:defs.bzl`.

```python
load("@bzd_rules_nodejs//nodejs:defs.bzl",
    "bzd_nodejs_library",
    "bzd_nodejs_binary",
    "bzd_nodejs_test",
    "bzd_nodejs_web_binary",
    "bzd_nodejs_web_library",
    "bzd_nodejs_oci",
    "bzd_nodejs_requirements_compile",
)
```

| Rule                              | Purpose                                                    |
| --------------------------------- | ---------------------------------------------------------- |
| `bzd_nodejs_library`              | Reusable library (srcs, deps, packages)                    |
| `bzd_nodejs_binary`               | Runnable Node.js server binary                             |
| `bzd_nodejs_test`                 | Node.js test — runs under Mocha automatically              |
| `bzd_nodejs_web_binary`           | Vite-bundled frontend app (produces a `.bundle` directory) |
| `bzd_nodejs_web_library`          | Vite-compiled UMD/CJS library                              |
| `bzd_nodejs_oci`                  | Wraps a Node.js binary into a Docker/OCI image             |
| `bzd_nodejs_requirements_compile` | Converts `requirements.in` → `requirements.json` lockfile  |
| `bzd_nodejs_static`               | Generates static files from a Node.js script               |

**Key design**: `bzd_nodejs_binary` and `bzd_nodejs_test` are macros that create a `<name>.install` target first (runs pnpm offline to populate `node_modules`), then the actual binary/test target.

---

## Imports and the `#bzd/` Prefix

The `#bzd/` import prefix resolves to the **monorepo root**. Use it for all cross-package imports (any import referencing a file outside the current package/directory).

```javascript
// Cross-package import — ALWAYS use #bzd/
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import LogFactory from "#bzd/nodejs/core/log.mjs";
import { HttpClient } from "#bzd/nodejs/core/http/client.mjs";
import config from "#bzd/nodejs/vue/apps/config.json" with { type: "json" };

// Relative import — OK within the same package/directory
import Format from "./format.mjs";
import Router from "../router.mjs";

// npm package import — bare package name, no prefix
import { Command } from "commander/esm.mjs";
```

**Rule of thumb:**

- Same directory or subdirectory → use relative (`./`, `../`)
- Different package (anywhere else in the repo) → use `#bzd/`
- npm packages → use the bare package name

---

## Code Style

- **File extension**: `.mjs` (ES Modules) for all new code
- **Indentation**: tabs (not spaces) — enforced by Prettier
- **Column limit**: 120 characters
- **Quotes**: double quotes (`"`) — no single quotes
- **Semicolons**: required
- **Arrow functions**: always use parentheses around parameters (`(x) => x`)
- **Functions/variables**: `camelCase`
- **Classes**: `PascalCase`
- **Private members**: trailing underscore (e.g., `value_`, `helperMethod_()`)
- **No `console.log`**: use `LogFactory` instead (see Logging section)

### Prettier settings (`tools/nodejs/.prettierrc.json`)

```json
{
  "arrowParens": "always",
  "bracketSpacing": true,
  "printWidth": 120,
  "semi": true,
  "singleQuote": false,
  "useTabs": true,
  "vueIndentScriptAndStyle": true
}
```

---

## ExceptionFactory Pattern

All modules create their own typed exception class using `ExceptionFactory`. This is the standard error-handling mechanism — never throw raw `Error` objects.

```javascript
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("mymodule", "submodule");

// Throw an error
Exception.error("Something went wrong: {}", detail);

// Assert a condition
Exception.assert(value > 0, "Value must be positive, got {}", value);

// Assert equality (deep comparison — works on objects, arrays, sets)
Exception.assertEqual(result, expected);

// Assert a precondition (user input validation)
Exception.assertPrecondition(input !== null, "Input must not be null");

// Assert that an async block throws with a matching message
await Exception.assertThrowsWithMatch(async () => {
  await riskyOperation();
}, "expected error substring");

// Wrap a caught Error with context
const e = Exception.fromError(caughtError, "Context: {}", contextInfo);
```

---

## Logging

```javascript
import LogFactory from "#bzd/nodejs/core/log.mjs";

const Log = LogFactory("mymodule");

Log.info("Server started on port {}", port);
Log.warning("Deprecated feature used: {}", feature);
Log.error("Operation failed: {}", error);
Log.debug("Processing item {}", item);
```

---

## Writing a New Library

1. Create `nodejs/<module>/my_feature.mjs`:

```javascript
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("mymodule", "myfeature");

export class MyFeature {
  constructor(config) {
    this.config_ = config;
  }

  doSomething(value) {
    Exception.assert(value !== null, "Value must not be null");
    return value * 2;
  }
}

export default MyFeature;
```

2. Create `nodejs/<module>/BUILD.bazel`:

```python
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
	name = "my_feature",
	srcs = ["my_feature.mjs"],
	visibility = ["//visibility:public"],
	deps = [
		"//nodejs/core:exception",
		"//nodejs/core:log",
	],
)
```

3. Reference from another target's BUILD file:
   - `"//nodejs/<module>:my_feature"`

---

## Writing Tests

Tests use Mocha and are named `<name>_test.mjs`. Mocha globals (`describe`, `it`) are available automatically — no import needed.

### Test file structure

```javascript
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";
import MyFeature from "#bzd/nodejs/mymodule/my_feature.mjs";

const Exception = ExceptionFactory("test", "myfeature");

describe("MyFeature", () => {
  describe("doSomething", () => {
    it("doubles a positive number", () => {
      const f = new MyFeature({});
      Exception.assertEqual(f.doSomething(3), 6);
    });

    it("throws on null input", () => {
      const f = new MyFeature({});
      Exception.assertThrowsWithMatch(() => {
        f.doSomething(null);
      }, "Value must not be null");
    });
  });

  describe("async behavior", () => {
    it("resolves correctly", async () => {
      const result = await f.asyncMethod();
      Exception.assertEqual(result, expectedValue);
    });
  });
});
```

### Single test BUILD.bazel

```python
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

bzd_nodejs_test(
	name = "my_feature",
	srcs = ["my_feature_test.mjs"],
	main = "my_feature_test.mjs",
	deps = [
		"//nodejs/mymodule:my_feature",
		"//nodejs/core:exception",
	],
)
```

### Bulk-generate tests from all `*_test.mjs` files (preferred pattern)

```python
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_test")

test_srcs = glob(["*_test.mjs"])

[bzd_nodejs_test(
	name = src.replace("_test.mjs", ""),
	srcs = [src],
	main = src,
	deps = [
		"//nodejs/mymodule:my_feature",
		"//nodejs/core:exception",
	],
) for src in test_srcs]
```

### Test with npm packages

```python
bzd_nodejs_test(
	name = "my_test",
	srcs = ["my_test.mjs"],
	main = "my_test.mjs",
	packages = ["@nodejs_deps//:some_package"],
	deps = ["//nodejs/core:exception"],
)
```

---

## Writing a Full Application (Backend + Frontend)

### Minimal app structure

```
apps/my_app/
├── BUILD.bazel
├── api.json              # REST/WebSocket API schema
├── backend.mjs           # Node.js HTTP server entry point
└── frontend/
    ├── BUILD.bazel
    ├── app.mjs           # Frontend entry point
    └── app.vue           # Root Vue component
```

### `BUILD.bazel` (backend)

```python
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_binary")

bzd_nodejs_binary(
	name = "backend",
	srcs = ["backend.mjs", "api.json"],
	args = [
		"--static",
		"$(rootpath //apps/my_app/frontend:frontend).bundle",
	],
	data = ["//apps/my_app/frontend:frontend"],
	main = "backend.mjs",
	deps = ["//nodejs/vue/apps:backend"],
)
```

### `BUILD.bazel` (frontend)

```python
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_web_binary")

bzd_nodejs_web_binary(
	name = "frontend",
	srcs = ["app.mjs", "app.vue"],
	config_scss = "//nodejs/styles:default.scss",
	main = "app.mjs",
	deps = ["//nodejs/vue/apps:frontend"],
)
```

### `backend.mjs` skeleton

```javascript
import APIv1 from "#bzd/api.json" with { type: "json" };
import Backend from "#bzd/nodejs/vue/apps/backend.mjs";

const backend = await Backend.make(APIv1).useAuthentication().useLogger().setup();
await backend.start();
```

### `frontend/app.mjs` skeleton

```javascript
import APIv1 from "#bzd/api.json" with { type: "json" };
import Frontend from "#bzd/nodejs/vue/apps/frontend.mjs";
import App from "./app.vue";

const frontend = Frontend.make(App).useRest(APIv1.rest).useAuthentication().useLogger().setup();
frontend.mount("#app");
```

---

## Vue Components

Vue components use `.vue` single-file component (SFC) format with `<template>`, `<script>`, and optionally `<style>`.

```vue
<template>
  <div class="bzd-my-component">
    <slot name="header"></slot>
    <div class="bzd-my-component-content">{{ message }}</div>
  </div>
</template>

<script>
import ExceptionFactory from "#bzd/nodejs/core/exception.mjs";

const Exception = ExceptionFactory("components", "mycomponent");

export default {
  props: {
    message: { type: String, required: true },
  },
  data() {
    return {
      value_: null,
    };
  },
  methods: {
    handleClick() {
      Exception.assert(this.message, "Message must not be empty");
    },
  },
};
</script>

<style lang="scss">
.bzd-my-component {
  display: flex;
}
</style>
```

### Vue component BUILD.bazel

```python
load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
	name = "my_component",
	srcs = ["my_component.vue"],
	visibility = ["//visibility:public"],
	deps = [
		"//nodejs:vue",
		"//nodejs/core:exception",
	],
)
```

---

## Adding npm Dependencies

All npm packages are declared in `tools/nodejs/requirements.in` and locked in `tools/nodejs/requirements.json`.

1. Add the package name to `tools/nodejs/requirements.in`:

   ```
   my-package
   my-package=1.2.3
   ```

2. Regenerate the lockfile:

   ```bash
   ./tools/bazel run //tools/nodejs:requirements
   ```

3. Use the package in a BUILD file via the `packages` attribute:

   ```python
   bzd_nodejs_library(
   	name = "my_lib",
   	srcs = ["my_lib.mjs"],
   	packages = ["@nodejs_deps//:my_package"],
   	visibility = ["//visibility:public"],
   )
   ```

4. Import in code using the bare package name (no `#bzd/` prefix):
   ```javascript
   import { something } from "my-package";
   import { Command } from "commander/esm.mjs";
   ```

---

## Formatting and Static Analysis

```bash
# Run the sanitizer (Prettier + linting) on changed files
./tools/bazel run //:sanitizer

# Run on ALL files in the repo
./tools/bazel run //:sanitizer -- --all

# Check-only mode (report issues, no writes)
./tools/bazel run //:sanitizer -- --check --all
```

Prettier is applied to `.mjs`, `.cjs`, `.js`, `.vue`, `.css`, and `.scss` files.

To exclude a directory from sanitizer checks, place a `.sanitizerignore` file in that directory.

---

## CI / Quality Gate

```bash
# Full quality gate (tests + sanitizer)
./quality_gate.sh

# Test all Node.js targets
./tools/bazel test //nodejs/...

# Test all app targets
./tools/bazel test //apps/...

# Sanitizer check only
./tools/bazel run //:sanitizer -- --check --all
```

Always run `./tools/bazel run //:sanitizer` before committing.
