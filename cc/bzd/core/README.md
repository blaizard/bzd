# Core

Platform-agnostic building blocks used across all bzd applications: error handling, assertions, logging, coroutines, streams and serialization.

## Key Points

- **Async-first** - built around C++20 coroutines (`bzd::Async`).
- **Zero allocation** - relies on fixed-capacity containers.
- **Platform agnostic** - platform-specific behavior is provided by backends.

## Components

- `error` - The lightweight `bzd::Error` type and helpers (`bzd::error::Failure`, `Timeout`, `Busy`, `Eof`, `Data`).
- `channel` - Input/output channels (`bzd::OStream`, `bzd::IStream`, `bzd::IOStream`).
- `print` - Helpers to print to a stream.
- `units` - Strongly-typed length and time units with literals.
- `assert` - Runtime assertions that panic on failure.
- `async` - Coroutines: `bzd::Async`, `bzd::Generator`, executors and composition.
- `io` - Async buffers with source/sink access.
- `logger` - Leveled logging to an output stream.
- `serialization` - Binary serialization of built-in types.
