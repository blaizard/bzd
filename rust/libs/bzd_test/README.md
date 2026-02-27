# Test Framework for Rust

This is a simple test framework for rust, it is made to be simple and run on any platform (including embedded).

## Usage

```rust
#![no_std]

#[cfg(test)]
#[bzd_test::test]
mod tests {
    #[test]
    fn test_dummy() {
        assert_eq!(1, 1);
    }
}
```

BUILD.bazel

```py
load("//rust/libs/bzd_test:defs.bzl", "rust_test")

rust_library(
    name = "lib",
    srcs = [
        "lib.rs",
    ],
)

rust_test(
    name = "test_crate",
    create = ":lib",
)
```
