# BDL Rust backend

The Rust extension for the BDL (Bzd Description Language) framework. It provides the Bazel rules and code generators that turn BDL definitions into Rust code.

## Key Points

- **Rust code generation** - generates traits from `.bdl` interface definitions.
- **Bazel integration** - custom rules that embed BDL into the Rust build.
- **Namespace-aware symbols** - generated symbols are prefixed with the BDL namespace.

## Usage

BDL definitions are consumed by depending on a `bdl_library` target from any `rust_library`:

```python
bdl_library(
    name = "interface",
    srcs = ["interface.bdl"],
)

rust_library(
    name = "implementation",
    srcs = ["implementation.rs"],
    deps = [":interface"],
)
```

## Library

The Rust generator converts BDL definitions into a `no_std` Rust file. Symbols are prefixed with the BDL namespace of the file, joined with `_`, and formatted as CamelCase. For example, given the following interface in `interface.bdl`:

```bdl
namespace bzd.test;

interface MyInterface {
	method add(a = const Integer, b = const Integer) -> Integer;
}
```

The generated code provides:

```rust
pub trait BzdTestMyInterface {
    fn add(&self, a: i32, b: i32) -> i32;
}
```

The implementation implements the trait:

```rust
use rust_bdl_tests_interface_interface::BzdTestMyInterface;

struct Calculator;

impl BzdTestMyInterface for Calculator {
    fn add(&self, a: i32, b: i32) -> i32 {
        a + b
    }
}
```

## Crate naming

The generated crate is named after the full label of the `bdl_library` target, with the package path and target name joined with `_`. This keeps crate names unique across packages. For example, `//hello:interface` generates the crate `hello_interface`, and `//rust/bdl/tests/interface:interface` generates `rust_bdl_tests_interface_interface`.

## Enums

Top-level enums are generated as Rust enums with `repr(u8)`, standard derives, and CamelCase names and variants. For example, given the following enum in `interface.bdl`:

```bdl
namespace bzd.components.esp;

enum UartDevice {
	uart0
,	uart1
}
```

The generated code provides:

```rust
#[repr(u8)]
#[derive(Copy, Clone, PartialEq, Eq, Debug)]
pub enum BzdComponentsEspUartDevice {
    Uart0,
    Uart1,
}
```

## Dependencies

When a `bdl_library` declares `deps`, the generated Rust file imports the symbols of each dependency privately:

```python
bdl_library(
    name = "file_b",
    srcs = ["file_b.bdl"],
    deps = [":file_a"],
)
```

The generated code for `file_b` starts with `use file_a::*;`, importing `file_a`'s symbols for internal use without re-exporting them. Downstream users must depend on `file_a` directly to access its symbols.

Rust dependencies provided through the `implementation` attribute are re-exported as part of the public API:

```python
bdl_library(
    name = "interface",
    srcs = ["interface.bdl"],
    implementation = {"rust": ":stream_impl"},
)
```

The generated code re-exports the implementation crate with `pub use stream_impl::*;`, publishing its symbols to downstream users.
