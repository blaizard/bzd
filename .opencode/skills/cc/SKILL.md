---
name: cc
description: How to write, build, test, and contribute C++ code in the bzd monorepo
compatibility: opencode
---

## Overview

- **C++ standard**: C++20 (concepts, coroutines, `constexpr` improvements)
- **Build system**: Bazel via `./tools/bazel` — use `//cc/bdl:cc.bzl` custom macros, never raw `cc_binary`
- **Toolchain**: Clang/GCC managed by `bzd_toolchain_cc` (hermetic — never install global compilers)
- **Memory model**: zero dynamic allocation — no `new`, `malloc`, `std::make_shared`, or STL heap containers
- **Types**: always use `bzd::` types from `cc/bzd/platform/types.hh`, not raw C++ or STL types
- **File extensions**: `.hh` for headers, `.cc` for implementation

---

## Project Structure

| Path                   | Purpose                                                                                   |
| ---------------------- | ----------------------------------------------------------------------------------------- |
| `cc/bzd/`              | Core standard library (containers, algorithms, async, platform types, etc.)               |
| `cc/bzd/container/`    | Fixed-size containers: `Array`, `Vector`, `String`, `Result`, `Optional`, `Variant`, etc. |
| `cc/bzd/core/`         | Core abstractions: async coroutines, error types, logger, assert, IO                      |
| `cc/bzd/algorithm/`    | Generic algorithms: copy, sort, find, fill, etc.                                          |
| `cc/bzd/platform/`     | Platform types (`bzd::Size`, `bzd::UInt8`, etc.)                                          |
| `cc/bzd/test/`         | Test framework: `TEST`, `TEST_ASYNC`, `EXPECT_*`, `ASSERT_*` macros                       |
| `cc/bzd/type_traits/`  | Type trait utilities                                                                      |
| `cc/bzd/utility/`      | General utilities: move, forward, ranges, synchronization, etc.                           |
| `cc/components/`       | Platform-specific and generic components composed via BDL                                 |
| `cc/components/posix/` | POSIX-specific components (proactor, network, stream)                                     |
| `cc/components/linux/` | Linux-specific components (epoll proactor)                                                |
| `cc/libs/`             | Higher-level libraries (HTTP, pthread, timer)                                             |
| `cc/targets/`          | Platform entry points (linux, posix, esp32, esp32s3)                                      |

---

## Running, Testing, and Building

```bash
# Build all C++ targets
./tools/bazel build //cc/...

# Test all C++ targets
./tools/bazel test //cc/...

# Test a specific target
./tools/bazel test //cc/bzd/core/async/tests:async
./tools/bazel test //cc/bzd/container/tests:array

# Show full test output even on success
./tools/bazel test --test_output=all //cc/bzd/container/tests:array

# Run with sanitizers
./tools/bazel test --config=asan //cc/...
./tools/bazel test --config=tsan //cc/...
./tools/bazel test --config=ubsan //cc/...

# Debug build (debug symbols + environment)
./tools/bazel test --config=debug //cc/bzd/core/async/tests:async
```

---

## Bazel Rules

All C++ targets in `bzd` use custom rules from `cc/bdl/cc.bzl`. There are two categories:

### Custom bzd rules (for new bzd library/test/binary code)

```python
load("//cc/bdl:cc.bzl", "bzd_cc_library", "bzd_cc_test", "bzd_cc_binary")
```

| Rule             | Purpose                                                                                                   |
| ---------------- | --------------------------------------------------------------------------------------------------------- |
| `bzd_cc_library` | Aggregates headers + deps into an umbrella library; generates a single `.hh` including all public headers |
| `bzd_cc_test`    | Wraps `bzd_runner_test`; links to the default platform target automatically                               |
| `bzd_cc_binary`  | Wraps a BDL system binary; requires a `target` (defaults to `//cc/targets:auto`)                          |

### Plain Bazel `cc_library` (for fine-grained individual header-only leaves)

```python
load("@rules_cc//cc:defs.bzl", "cc_library")
```

Used for individual single-header targets (e.g., each `.hh` in `cc/bzd/algorithm/`). The `bzd_cc_library` aggregation target then depends on these individual targets.

---

## Types

Always use `bzd` platform types from `cc/bzd/platform/types.hh` (included via `//cc/bzd/platform:types`):

| `bzd` type                                                   | Meaning                                 |
| ------------------------------------------------------------ | --------------------------------------- |
| `bzd::Bool`                                                  | `bool`                                  |
| `bzd::Size`                                                  | `std::size_t` (unsigned, pointer-sized) |
| `bzd::Int8` / `bzd::Int16` / `bzd::Int32` / `bzd::Int64`     | Signed fixed-width integers             |
| `bzd::UInt8` / `bzd::UInt16` / `bzd::UInt32` / `bzd::UInt64` | Unsigned fixed-width integers           |
| `bzd::Float32`                                               | `float` (32-bit)                        |
| `bzd::Float64`                                               | `double` (64-bit)                       |
| `bzd::Byte`                                                  | `std::byte`                             |
| `bzd::StringView`                                            | String view (non-owning)                |

---

## Code Style

- **Braces**: Allman style — opening brace on a new line
- **Indentation**: tabs (not spaces)
- **Column limit**: 120 characters
- **Namespaces**: wrap all `bzd` core code in `namespace bzd { ... }`
- **Functions / variables**: `camelCase`
- **Types** (classes, structs, enums): `PascalCase`
- **Private / non-public members**: trailing underscore (e.g., `value_`, `helperMethod_()`)
- **`[[nodiscard]]`**: required on all functions that return a value
- **`constexpr`**: prefer everywhere applicable
- **No `#include <iostream>`**: use `bzd::logger` or `bzd::format`
- **No STL containers**: use `bzd::Array`, `bzd::Vector`, `bzd::String`, etc.
- **No dynamic allocation**: no `new`, `malloc`, `std::make_shared`, `std::vector` with heap growth

### Example header

```cpp
#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

/// \brief A simple fixed-size counter.
template <bzd::Size maxCount>
class Counter
{
public: // Traits.
	using Self = Counter<maxCount>;

public:
	/// \brief Increment and return the new count.
	[[nodiscard]] constexpr bzd::Size increment() noexcept
	{
		++count_;
		return count_;
	}

	[[nodiscard]] constexpr bzd::Size get() const noexcept { return count_; }

private:
	bzd::Size count_{0u};
};

} // namespace bzd
```

---

## Error Handling

Two error return styles coexist depending on context:

### With `bzd::Error` (for components and libs using the platform error type)

```cpp
#include "cc/bzd/core/error.hh"

// Return a result wrapping bzd::Error
bzd::Result<int, bzd::Error> myFunction()
{
	if (failed)
		return bzd::error::Failure{"Something went wrong: '{}'."_csv, detail};
	return 42;
}

// Result<void> success
bzd::Result<void, bzd::Error> myVoidFunction()
{
	if (failed)
		return bzd::error::Failure{"Reason"_csv};
	return bzd::nullresult;
}
```

Available error type constructors:

| Constructor                | `ErrorType`                              |
| -------------------------- | ---------------------------------------- |
| `bzd::error::Failure{...}` | `failure` — generic failure              |
| `bzd::error::Timeout{...}` | `timeout` — operation timed out          |
| `bzd::error::Busy{...}`    | `busy` — resource busy, can retry        |
| `bzd::error::Eof{...}`     | `eof` — end of stream/channel            |
| `bzd::error::Data{...}`    | `data` — data corruption or invalid data |

### With custom error enums (for lower-level containers/utilities)

```cpp
enum class MyError { notFound, overflow };
using MyResult = bzd::Result<int, MyError>;

MyResult myFunction()
{
	if (failed)
		return bzd::error::make(MyError::notFound);
	return 42;
}
```

---

## Async / Coroutines

```cpp
#include "cc/bzd/core/async.hh"

// Async function returning a value
bzd::Async<int> compute()
{
	co_await bzd::async::yield();
	co_return 42;
}

// Async function returning void
bzd::Async<> doWork()
{
	co_await !compute();  // '!' propagates error automatically
	co_return {};
}

// Await and handle result manually
bzd::Async<> withResult()
{
	const auto result = co_await compute();
	if (!result) { /* handle error */ }
	co_return {};
}

// Async returning an error
bzd::Async<int> failingAsync()
{
	co_return bzd::error::Failure{"Async error"_csv};
}
```

- Return type: `bzd::Async<T>` (or `bzd::Async<>` for void)
- Use `co_await` to suspend/resume and `co_return` to return a value
- `co_await !expr` propagates errors automatically (fatal propagation)
- `co_await expr` returns a `Result` to inspect manually

---

## Adding a New Library

1. Create the header: `cc/bzd/<module>/my_feature.hh`

```cpp
#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

class MyFeature
{
public:
	[[nodiscard]] constexpr bzd::Size compute() const noexcept;

private:
	bzd::UInt32 value_{};
};

} // namespace bzd
```

2. Create `cc/bzd/<module>/BUILD.bazel`:

```python
load("@rules_cc//cc:defs.bzl", "cc_library")
load("//cc/bdl:cc.bzl", "bzd_cc_library")

# Umbrella target — depends on all sub-targets in this module
bzd_cc_library(
    name = "my_module",
    visibility = ["//visibility:public"],
    deps = [":my_feature"],
)

# Individual fine-grained target
cc_library(
    name = "my_feature",
    hdrs = ["my_feature.hh"],
    visibility = ["//visibility:public"],
    deps = ["//cc/bzd/platform:types"],
)
```

3. Reference in another BUILD file:
   - Within `cc/bzd/`: `"//cc/bzd/<module>:my_feature"`
   - From outside `cc/`: `"//cc/bzd/<module>:my_feature"`

---

## Writing Tests

Tests use the custom `bzd` test framework (`cc/bzd/test/test.hh`). Test files use the `.cc` extension.

### Synchronous test

```cpp
#include "cc/bzd/test/test.hh"
#include "cc/bzd/<module>/my_feature.hh"

TEST(MyFeature, BasicBehavior)
{
	bzd::MyFeature f{};
	EXPECT_EQ(f.compute(), 0u);
}

TEST(MyFeature, EdgeCase)
{
	bzd::MyFeature f{};
	EXPECT_FALSE(f.isEmpty());
}
```

### Asynchronous test

```cpp
#include "cc/bzd/test/test.hh"
#include "cc/bzd/core/async.hh"

TEST_ASYNC(MyFeature, AsyncBehavior)
{
	const auto result = co_await myAsync();
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), 42);
	co_return {};
}
```

### Test assertion macros

| Macro                                       | Behavior                        |
| ------------------------------------------- | ------------------------------- |
| `EXPECT_TRUE(cond)` / `ASSERT_TRUE(cond)`   | Checks condition is true        |
| `EXPECT_FALSE(cond)` / `ASSERT_FALSE(cond)` | Checks condition is false       |
| `EXPECT_EQ(a, b)` / `ASSERT_EQ(a, b)`       | Checks `a == b`                 |
| `EXPECT_NE(a, b)` / `ASSERT_NE(a, b)`       | Checks `a != b`                 |
| `EXPECT_LT/LE/GT/GE(a, b)`                  | Comparison checks               |
| `EXPECT_NEAR(a, b, eps)`                    | Floating-point near-equality    |
| `EXPECT_STREQ(s1, s2)`                      | C-string equality               |
| `EXPECT_EQ_RANGE(r1, r2)`                   | Range/container equality        |
| `EXPECT_ANY_THROW(expr)`                    | Expression throws any exception |

`ASSERT_*` variants abort the test immediately on failure; `EXPECT_*` variants continue.
`ASSERT_ASYNC_*` variants must be used inside `TEST_ASYNC` coroutines.

### Test BUILD.bazel

```python
load("//cc/bdl:cc.bzl", "bzd_cc_test")

bzd_cc_test(
    name = "my_feature",
    srcs = ["my_feature.cc"],
    deps = [
        "//cc/bzd/<module>:my_feature",
        "//cc/bzd/test",
    ],
)
```

### Bulk-generate tests from multiple `.cc` files

```python
load("//cc/bdl:cc.bzl", "bzd_cc_test")

[bzd_cc_test(
    name = path.replace(".cc", ""),
    srcs = [path],
    deps = [
        "//cc/bzd/<module>",
        "//cc/bzd/test",
    ],
) for path in glob(["*.cc"])]
```

### Platform-constrained tests

```python
bzd_cc_test(
    name = "linux_only_test",
    srcs = ["linux_only.cc"],
    target_compatible_with = ["@bzd_platforms//al:linux"],
    deps = [
        "//cc/bzd/test",
    ],
)
```

---

## Platforms and Targets

| Bazel target                | Platform                                                                   |
| --------------------------- | -------------------------------------------------------------------------- |
| `//cc/targets:auto`         | Auto-selects based on host (default for `bzd_cc_test` and `bzd_cc_binary`) |
| `//cc/targets/linux:auto`   | Linux (epoll proactor, x86_64, clang/gcc)                                  |
| `//cc/targets/posix:auto`   | POSIX (sync proactor, OSX ARM64 clang)                                     |
| `//cc/targets/esp32:auto`   | ESP32 (Xtensa LX6)                                                         |
| `//cc/targets/esp32s3:auto` | ESP32-S3 (Xtensa LX7)                                                      |

Platform constraints use the form `@bzd_platforms//al:<platform>` (e.g., `linux`, `posix`).

To target a specific platform explicitly:

```python
bzd_cc_test(
    name = "my_test",
    srcs = ["my_test.cc"],
    target = "//cc/targets/linux:auto",
    deps = ["//cc/bzd/test"],
)
```

---

## Formatting and Static Analysis

```bash
# Run the sanitizer (clang-format + linting) on changed files
./tools/bazel run //:sanitizer

# Run on ALL files in the repo
./tools/bazel run //:sanitizer -- --all

# Check-only mode (report issues, no writes)
./tools/bazel run //:sanitizer -- --check --all

# Run clang-tidy static analysis on C++ targets
./tools/bazel build --config=clang-tidy //cc/...

# Run clang-tidy on a specific target
./tools/bazel build --config=clang-tidy //cc/bzd/container/...
```

To exclude a directory from sanitizer checks, place a `.sanitizerignore` file in that directory.

---

## CI / Quality Gate

```bash
# Full quality gate (tests + sanitizer + static analysis)
./quality_gate.sh

# Test all C++ targets
./tools/bazel test //cc/...

# Sanitizer check only
./tools/bazel run //:sanitizer -- --check --all

# clang-tidy check only
./tools/bazel build --config=clang-tidy //cc/...
```

Always run `./tools/bazel run //:sanitizer` before committing.
