# Pattern String Syntax

A pattern string is a compile-time string that drives formatting and parsing — the core input for
`bzd::toString`, `bzd::toStream`, `bzd::fromString`, and `bzd::fromStream`.

It is declared with the `_csv` literal to obtain a compile-time string view:

```c++
"Hello {}"_csv
```

## Replacement Fields

Arguments are inserted at the position of a replacement field, delimited by curly braces `{...}`:

| Field        | Description                                                              |
| ------------ | ------------------------------------------------------------------------ |
| `{}`         | Automatically indexed field, arguments are consumed in order.            |
| `{0}`, `{1}` | Explicitly indexed field, references the argument at the given position. |
| `{0:spec}`   | Explicitly indexed field with a format specification (see below).        |
| `{:spec}`    | Automatically indexed field with a format specification.                 |
| `{{` `}}`    | Escaped curly braces, rendered as a single `{` or `}` character.         |

```c++
bzd::toString(range, "Hello {1} {0:d}"_csv, 12, -89); // "Hello -89 12"
bzd::toString(range, "Literal: {{ }}"_csv);           // "Literal: { }"
```

An argument can be referenced multiple times and in any order. Every argument must appear at
least once in the pattern, this is enforced at compile time.

## Format Specification

The format specification is the optional text between the optional index and the closing `}`.
Its grammar and the options it accepts depend on the type of the referenced argument:

```
format_spec ::= [sign][#][.precision][type]
```

### Integral types

| Option | Description                                                |
| ------ | ---------------------------------------------------------- |
| `+`    | Always prefix the number with its sign.                    |
| `-`    | Prefix the number with its sign only when it is negative.  |
| `#`    | Alternate form, prefixes the output with `0b`, `0o`, `0x`. |
| `b`    | Binary representation.                                     |
| `d`    | Decimal representation (default).                          |
| `o`    | Octal representation.                                      |
| `x`    | Lowercase hexadecimal representation.                      |
| `X`    | Uppercase hexadecimal representation.                      |

```c++
bzd::toString(range, "{} == {0:#b} == {0:#o} == {0:#x} == {0:#X}"_csv, 42);
// "42 == 0b101010 == 0o52 == 0x2a == 0x2A"
```

### Floating point types

| Option       | Description                                              |
| ------------ | -------------------------------------------------------- |
| `+` / `-`    | Same sign control as for integral types.                 |
| `.precision` | Number of decimals to print (default is 6).              |
| `f`          | Fixed point representation.                              |
| `%`          | Multiplies the value by 100 and appends a `%` character. |

```c++
bzd::toString(range, "Hello {:.1f}"_csv, 12.45);   // "Hello 12.5"
bzd::toString(range, "This {1} is {0:.1%}"_csv, 0.0349, "milk"); // "This milk is 3.5%"
```

### Bytes, strings and byte ranges

| Option       | Description                                     |
| ------------ | ----------------------------------------------- |
| `.precision` | Maximum number of bytes to output (truncation). |

```c++
bzd::toString(range, "Hello {:.2}"_csv, "World"); // "Hello Wo"
```

A `bzd::Byte` uses the integral specification but defaults to the alternate lowercase
hexadecimal form (`0x2a`). A `bzd::Bool` prints `true` or `false`, without options.

### Pointers and enums

| Type    | Behavior                                                                                                                                    |
| ------- | ------------------------------------------------------------------------------------------------------------------------------------------- |
| Pointer | Prints the address as lowercase hexadecimal, prefixed with `0x`.                                                                            |
| Enum    | Prints `name (value)` using the stringified enumerator name, or `<unknown> (value)`. Only automatically assigned enumerators are supported. |

```c++
bzd::toString(range, "Pointer {}"_csv, ptr);      // "Pointer 0x1234"
bzd::toString(range, "enum: {}"_csv, Color::red); // "enum: red (0)"
```

## Compile-Time Checks

The pattern is validated at compile time:

- The pattern must be well-formed (balanced braces, `}` only as part of a field or escaped).
- Explicit indexes must not exceed the number of arguments.
- Format options must be valid for the argument type.
- Every argument must appear at least once.
