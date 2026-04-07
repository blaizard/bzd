---
name: bdl
description: How to read, write, and wire BDL (Bzd Description Language) files and their Bazel rules in the bzd monorepo — load this before touching any .bdl file or bdl_* Bazel rule
compatibility: opencode
---

## Mandatory usage

Load this skill before creating, editing, or reviewing any `.bdl` file or any `BUILD.bazel` that uses `bdl_library`, `bdl_target`, or `bdl_system`. It governs:

- BDL file-level declarations (namespace, use, extern)
- Entity types and their scopes (struct, enum, using, interface, component, composition)
- Built-in types and their contracts
- Expression, method, and contract syntax
- Composition meta-functions (connect, bind, list)
- Executor assignment rules
- Bazel rules: `bdl_library`, `bdl_target`, `bdl_system`
- The three-stage build pipeline (preprocess → generate → compose)

---

## What BDL is

BDL (Bzd Description Language) is a DSL for the bzd framework. It describes distributed systems, reduces boilerplate for interfaces, and enforces I/O contracts.

### Three-stage build pipeline

| Stage      | Input                            | Output                                    |
| ---------- | -------------------------------- | ----------------------------------------- |
| Preprocess | `.bdl` source files              | `.bdl.o` language-agnostic object files   |
| Generate   | `.bdl.o` + data file             | Language-specific files (e.g., C++ `.hh`) |
| Compose    | All `.bdl.o` + deps for a system | Full composition output for each target   |

All stages are hermetic and invoked via Bazel rules — never run the `bdl` CLI manually.

### Glossary

| Term        | Meaning                                                                                           |
| ----------- | ------------------------------------------------------------------------------------------------- |
| Application | A runnable workload whose lifetime controls the overall application lifetime                      |
| Service     | A workload whose lifetime does not affect the rest of the application                             |
| Workload    | An application or a service                                                                       |
| Core        | A single unit of execution                                                                        |
| Executor    | Minimal scheduling unit; may contain multiple workloads and run on multiple cores (work-stealing) |
| Platform    | Implicit dependencies a binary relies on                                                          |
| Target      | A group of executors + a single platform that produces a single binary                            |
| System      | A group of targets that constitute a complete deployable system                                   |
| Gateway     | Connection endpoint between executors                                                             |

---

## File-level declarations

Every `.bdl` file may contain the following declarations, which must appear before entity definitions:

| Keyword                 | Purpose                                                     | Constraints                          |
| ----------------------- | ----------------------------------------------------------- | ------------------------------------ |
| `namespace a.b.c;`      | Declares the namespace for all entities in this file        | One per file; must appear at the top |
| `use "path/to/f.bdl"`   | Imports another BDL file (workspace-root-relative path)     | Before entity declarations           |
| `extern interface Foo;` | Declares an interface implemented externally (e.g., in C++) | —                                    |
| `extern struct Foo;`    | Declares a struct implemented externally                    | —                                    |

```bdl
use "interfaces/timer.bdl"
use "interfaces/io.bdl"

namespace jardinier;
```

---

## Entity types

| Category    | Keyword       | Description                                       |
| ----------- | ------------- | ------------------------------------------------- |
| struct      | `struct`      | Data-only type with fields (no `config:` scope)   |
| enum        | `enum`        | Enumeration                                       |
| using       | `using`       | Strong typedef with optional contracts            |
| interface   | `interface`   | Abstract interface; supports inheritance          |
| component   | `component`   | Concrete implementation of one or more interfaces |
| composition | `composition` | Instantiation scope; top-level or named           |

### struct

```bdl
struct Coord {
	x = Float;
	y = Integer;
}
```

Fields live in the `interface:` scope implicitly. No `config:` block.

### enum

```bdl
enum LightState {
	on
,	off
,	storm
}
```

Reference enum values as `LightState.on`.

### using (strong typedef)

```bdl
using MyType = Integer [min(0) max(100)];
```

### interface

```bdl
interface Shape {
	using Area = Integer [min(0)];
	method surface() -> Area;
}

interface TimedShape : Shape {
	method tick();
}
```

### component

A component implements one or more interfaces and has three optional scopes:

```bdl
component MyComp : InterfaceA, InterfaceB {
config:
	using T = Any;            // template type parameter
	size = Integer [min(1)];  // required constructor argument
	dep = {out};              // preset-wired platform dependency

interface:
	method run();             // exposed method
	data = Integer;           // writable shared field
	result = const Float;     // read-only shared field

composition:
	this.run();               // register run() as a service workload
}
```

**`config:` scope** — describes how the component is instantiated:

- `using` entries become C++ template parameters.
- `expression` entries become C++ constructor arguments.
- `{preset}` entries (`{out}`, `{timer}`, `{in}`, …) are platform-wired at composition time.

**`interface:` scope** — public-facing fields and methods shared by all instances.

**`composition:` scope** — built-in workload registrations and sub-component wiring. `this` refers to the current component instance.

### composition (top-level)

```bdl
composition {
	emitter = Emitter();
	emitter.generate();
	poller  = Poller();
	poller.poll();
	connect(emitter.send, poller.receive);
}
```

### Named composition

```bdl
composition board {
	pwm0 = PWM(channel = 0);
}
```

Creates sub-namespace `board.*` (e.g., `platform.board.pwm0`). Used for platform-specific wiring.

---

## Built-in types

| Type        | Description                                                  |
| ----------- | ------------------------------------------------------------ |
| `Void`      | Empty type; used for return types                            |
| `Boolean`   | `true` / `false`                                             |
| `Integer`   | Arbitrary-precision integer; size set via contracts          |
| `Float`     | Floating-point number; size set via contracts                |
| `Byte`      | 8-bit value                                                  |
| `String`    | String value                                                 |
| `Result<T>` | Either a value of type `T` or an error                       |
| `Async<T>`  | Asynchronous promise returning `T` or an error               |
| `Array<T>`  | Fixed-length array; capacity set via `[capacity(n)]`         |
| `Vector<T>` | Resizable sequential container; capacity via `[capacity(n)]` |

---

## Expression syntax

```
[name [: InterfaceType] =] [const] Type<Template>(arg = value, ...) [contracts];
```

- **`name`** — optional instance name; required at component level.
- **`: InterfaceType`** — optional type annotation constraining the instance to an interface.
- **`const`** — marks the field read-only (other components can only read it).
- **Literals**: integers (`1`, `-4`), floats (`0.23`), booleans (`true`/`false`), strings (`"hello"`), enum values (`LightState.on`).
- **Symbol references**: relative (`componentName`) or fully-qualified (`a.b.c`).
- **Preset** (`{out}`, `{timer}`, `{in}`): platform-wired shortcut expanded at composition stage.

Examples:

```bdl
size = Integer [min(1)];                         // field with contract
timer = {timer};                                 // preset-wired dependency
water = WaterController(wateringTimeS = 60);     // instantiation with arg
square : Shape = Square(width = 12);             // interface-typed instance
result = const Float;                            // read-only field
```

---

## Method syntax

```
method name(param1 = Type [contracts], ...) [contracts] [-> ReturnType [contracts]];
```

Examples:

```bdl
method run();
method surface() -> Area;
method connect(host = String [mandatory], port = Integer [min(1) max(65535)]) -> Result<Void>;
```

---

## Contracts

Contracts are attached with `[contract1 contract2(val) ...]` and constrain values, types, or entities.

| Contract        | Role             | Description                                             |
| --------------- | ---------------- | ------------------------------------------------------- |
| `min(n)`        | Value / Template | Minimum value                                           |
| `max(n)`        | Value / Template | Maximum value                                           |
| `mandatory`     | Value / Template | Value must be provided; no default is allowed           |
| `capacity(n)`   | Meta             | Capacity of a container (`Array`, `Vector`)             |
| `integer`       | Value            | Assert value is an integer                              |
| `float`         | Value            | Assert value is a float                                 |
| `boolean`       | Value            | Assert value is a boolean                               |
| `string`        | Value            | Assert value is a string                                |
| `executor`      | Entity           | Tag a component instance as the executor for this scope |
| `executor(fqn)` | Entity           | Assign a component to a named executor                  |
| `init`          | Entity           | Tag a method as an initializer                          |
| `shutdown`      | Entity           | Tag a method as a shutdown handler                      |
| `convertible`   | Value            | Allow type conversion                                   |

```bdl
esp32 = Executor(core0, core1) [executor];
comp1 = Component [executor(linux)];
method start() [init];
using Port = Integer [min(1) max(65535)];
capacity = Integer(100) [min(1)];
```

---

## Composition meta-functions

These built-in functions are only valid inside `composition { }` blocks.

| Function                              | Description                                              |
| ------------------------------------- | -------------------------------------------------------- |
| `connect(source, sink)`               | Wire a source field to a sink field (many sinks allowed) |
| `bind(symbol_or_regex) [executor(n)]` | Assign a symbol or regex-matched set to a named executor |
| `list(values...)`                     | Create a list of values                                  |

```bdl
composition {
	connect(emitter.send, poller.receive);
	connect(lights.pwmDuty0, platform.board.pwm0.duty);

	bind(emitter)    [executor(hpc.executor)];
	bind(/poller.*/) [executor(esp32.executor)];
}
```

---

## Executors

Executors are the minimal scheduling unit. Cores inside an executor share a work queue (work-stealing).

### Single executor

When the system has only one executor, all workloads are assigned to it automatically — no explicit assignment needed.

### Multiple executors

Every workload must be explicitly assigned using:

1. Inline contract on the instance:

```bdl
comp1 = Component [executor(linux)];
```

2. Or `bind()` separately (supports FQN or regex):

```bdl
bind(emitter)    [executor(hpc.executor)];
bind(/poller.*/) [executor(esp32.executor)];
```

An error is raised at composition time if a workload is unassigned when multiple executors exist.

### Declaring an executor

```bdl
composition {
	linux = LinuxExecutor [executor];
}
```

or (ESP32 multi-core):

```bdl
composition {
	core0 = FreeRTOS(stackSize = 4000);
	core1 = FreeRTOS(stackSize = 10000);
	esp32 = Executor(core0, core1) [executor];
}
```

---

## Bazel rules

Load statement:

```python
load("@bzd_bdl//:defs.bzl", "bdl_library", "bdl_target", "bdl_system")
```

### `bdl_library`

Compiles `.bdl` source files into language-specific headers (e.g., C++ `.hh`). Use for interface definitions and component definitions.

```python
bdl_library(
    name = "timer",
    srcs = ["timer.bdl"],
    visibility = ["//visibility:public"],
    deps = ["//cc/bzd/platform/interfaces:timer"],
)
```

The generated `.hh` file has the same name as the target (e.g., `timer.hh`). Depend on the `bdl_library` target from `cc_library` or `bzd_cc_library` to consume the generated header.

### `bdl_target`

Defines a deployable binary target: composition files + deps + platform + language.

```python
# Top-level target (no platform — auto-detected from name ending in "auto")
bdl_target(
    name = "auto",
    composition = [":composition"],
    language = "cc",
    visibility = ["//visibility:public"],
    deps = [":main"],
)

# Platform-specific child (inherits composition, deps, language, binary from parent)
bdl_target(
    name = "x86_64_clang",
    parent = ":auto",
    platform = "@clang//:platform-linux-x86_64",
    visibility = ["//visibility:public"],
)
```

**Rules:**

- Targets whose names end with `auto` **must not** have a `platform` attribute.
- Targets with `parent =` **must not** set `language` or `binary`; they can only add `composition` files and `deps`.
- `language` must be one of the registered BDL language extensions (e.g., `"cc"`).

### `bdl_system`

Composes multiple `bdl_target`s into a full system and generates one binary per target entry.

```python
bdl_system(
    name = "system",
    targets = {
        "esp32": "@@//cc/targets/esp32:gcc",
        "hpc":   "@@//cc/targets/linux:x86_64_clang",
    },
    deps = [":lib"],
)
```

This produces two runnable binaries: `system.esp32` and `system.hpc`.

Under the hood, `bdl_system` runs one composition pass over all target BDL files and then creates one `_bdl_binary` per target entry with the appropriate platform transition.

### `bdl_application_factory`

Internal factory that wraps an application binary and injects BDL composition info. It is used by language-specific binary rules (e.g., `bzd_cc_binary`). Avoid using it directly unless writing a new language backend.

---

## Real-world patterns

### Pattern 1 — Pure interface library (no instantiation)

```bdl
namespace bzd;

interface Timer {
	method now() -> Integer;
	method nowUs() -> Integer;
}
```

```python
bdl_library(
    name = "timer",
    srcs = ["timer.bdl"],
    visibility = ["//visibility:public"],
)
```

### Pattern 2 — Component with config, interface, and self-registration

```bdl
namespace example;

component Emitter {
config:
	out   = {out};
	timer = {timer};

interface:
	method generate();
	send = Integer;
}
```

Top-level composition:

```bdl
composition {
	emitter = Emitter();
	emitter.generate();
}
```

### Pattern 3 — connect() wiring between components

```bdl
composition {
	water = WaterController(wateringTimeS = 60, wakeUpPeriodS = 43200);
	water.run();
	connect(water.pump, platform.board.ioWater.out);

	lights = LightController();
	lights.run();
	connect(lights.pwmDuty0, platform.board.pwm0.duty);
	connect(lights.pwmDuty1, platform.board.pwm1.duty);
}
```

### Pattern 4 — Sub-components (component composition)

```bdl
component RobotLeg : Kinematics {
config:
	actuator1 = Actuator;
	actuator2 = Actuator;
	logger    = Logger;

composition:
	connect(this.actuator1.error, logger);
	connect(this.actuator2.error, logger);
}
```

### Pattern 5 — Multi-target system with executor binding

```bdl
// composition.bdl
namespace example;

component Emitter {
config:
	out   = {out};
	timer = {timer};
interface:
	method generate();
	send = Integer;
}

component Poller {
config:
	out   = {out};
	timer = {timer};
interface:
	method poll();
	receive = const Integer;
}

composition {
	emitter = Emitter();
	emitter.generate();
	poller = Poller();
	poller.poll();
	connect(emitter.send, poller.receive);
	bind(emitter) [executor(hpc.executor)];
	bind(poller)  [executor(esp32.executor)];
}
```

```python
# BUILD.bazel
bdl_library(name = "composition", srcs = ["composition.bdl"])
cc_library(name = "lib", hdrs = ["simple.hh"], deps = [":composition", "//cc:bzd"])

bdl_system(
    name = "system",
    targets = {
        "esp32": "@@//cc/targets/esp32:gcc",
        "hpc":   "@@//cc/targets/linux:x86_64_clang",
    },
    deps = [":lib"],
)
```

---

## Constraints checklist

Before submitting any `.bdl` file or `BUILD.bazel` change, confirm:

- [ ] `namespace` is declared at the top of the file (before any entity declaration).
- [ ] All `use "..."` imports come before any entity declarations.
- [ ] All types referenced from other files are imported via `use "..."`.
- [ ] `bdl_library` is used for interface/component definitions; `bdl_target` for platform targets; `bdl_system` for full systems.
- [ ] Targets whose names end with `auto` have **no** `platform` attribute.
- [ ] Targets with `parent =` set **neither** `language` nor `binary`.
- [ ] `connect()` wiring uses fully qualified names when crossing namespaces.
- [ ] When the system has multiple executors, every workload is assigned via `[executor(name)]` or `bind()`.
- [ ] `const` is used on interface fields that should not be written by other components.
- [ ] `[mandatory]` is added to any config field that has no sensible default.
- [ ] Run `./tools/bazel run //:sanitizer` after any `.bdl` or `BUILD.bazel` change.
