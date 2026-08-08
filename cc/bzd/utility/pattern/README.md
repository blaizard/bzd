# Pattern

A compile-time framework for parsing and formatting strings with type specializations, enabling tools like `std::format`.

It splits a compile-time string into literals and `{...}` replacement fields, dispatches each field to a type specialization with optional metadata, all validated at compile time.

## Key Points

- **Compile-time validation**: malformed patterns, out-of-range indexes, invalid format options, and unused arguments are all rejected at build time.
- **No dynamic allocation**: patterns and formatting are resolved at compile time, making it safe for embedded and real-time contexts.
- **Symmetric formatting and parsing**: a single pattern drives both `toString`/`toStream` and `fromString`/`fromStream`.
- **Type specializations**: formatting and parsing are extensible to custom types via specializations, with optional metadata.
- **Lightweight**: part of the core library, with no external dependencies.

## Documentation

- [Pattern String Syntax](docs/syntax.md)
- [Formatting (toString / toStream)](docs/formatting.md)
- [Parsing (fromString / fromStream)](docs/parsing.md)
- [Custom Specializations](docs/specializations.md)
